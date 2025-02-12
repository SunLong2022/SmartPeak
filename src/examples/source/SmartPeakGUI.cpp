// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Ahmed Khalil, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <stdio.h>
#include <chrono>
#include <string>
#include <vector>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/ui/FilePicker.h>
#include <SmartPeak/ui/CreateSessionWidget.h>
#include <SmartPeak/ui/SessionFilesWidget.h>
#include <SmartPeak/ui/GuiAppender.h>
#include <SmartPeak/ui/Heatmap2DWidget.h>
#include <SmartPeak/ui/CalibratorsPlotWidget.h>
#include <SmartPeak/ui/ChromatogramPlotWidget.h>
#include <SmartPeak/ui/ChromatogramTICPlotWidget.h>
#include <SmartPeak/ui/ChromatogramXICPlotWidget.h>
#include <SmartPeak/ui/SpectraPlotWidget.h>
#include <SmartPeak/ui/OptionsWidget.h>
#include <SmartPeak/ui/SpectraMSMSPlotWidget.h>
#include <SmartPeak/ui/ParametersTableWidget.h>
#include <SmartPeak/ui/Report.h>
#include <SmartPeak/ui/WorkflowWidget.h>
#include <SmartPeak/ui/StatisticsWidget.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/ExplorerWidget.h>
#include <SmartPeak/ui/InfoWidget.h>
#include <SmartPeak/ui/RunWorkflowWidget.h>
#include <SmartPeak/ui/AboutWidget.h>
#include <SmartPeak/ui/LogWidget.h>
#include <SmartPeak/ui/SequenceTableWidget.h>
#include <SmartPeak/ui/LoadSessionWizard.h>
#include <SmartPeak/ui/LayoutLoader.h>
#include <SmartPeak/ui/SplitWindow.h>
#include <SmartPeak/core/EventDispatcher.h>
#include <SmartPeak/core/RawDataProcessors/StoreParameters.h>
#include <SmartPeak/core/RawDataProcessors/StoreValidationData.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureBackgroundQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDEstimations.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDFilters.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreFeatureRSDQCs.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreQuantitationMethods.h>
#include <SmartPeak/core/SequenceSegmentProcessors/StoreStandardsConcentrations.h>
#include <SmartPeak/core/ApplicationProcessors/SaveSession.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <filesystem>
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl2.h>
#include <misc/cpp/imgui_stdlib.h>
#include "service/services.hpp"

using namespace SmartPeak;

bool SmartPeak::enable_quick_help = true;
bool SmartPeak::run_on_server = false;

void initializeDataDirs(ApplicationHandler& state);

void initializeDataDir(
  ApplicationHandler& application_handler,
  const std::string& label,
  Filenames::Tag tag,
  const std::filesystem::path& default_dir
);

void checkTitles(const std::vector<std::shared_ptr<Widget>> windows);

std::string getMainWindowTitle(const ApplicationHandler& application_handler);

int main(int argc, char** argv)
// `int argc, char **argv` are required on Win to link against the proper SDL2/OpenGL implementation
{
  // to disable buttons, display info, and update the session cache
  bool workflow_is_done_ = true;
  bool file_loading_is_done_ = true;
  bool exceeding_table_size_ = false;
  bool ran_integrity_check_ = false;
  bool integrity_check_failed_ = false;
  bool RawDataAndFeatures_loaded_ = false;
  bool run_remote_workflow_ = true;
  ApplicationHandler application_handler_;
  SessionHandler session_handler_;
  WorkflowManager workflow_manager_;
  GuiAppender appender_;
  WorkflowClient workflow_client_;
  std::future<std::string> runworkflow_future_;
  SplitWindow split_window;
  LayoutLoader layout_loader(application_handler_);

  // EventDispatcher will dispatch events triggered by the observers in the main GUI thread
  EventDispatcher event_dispatcher;
  application_handler_.sequenceHandler_.addTransitionsObserver(&event_dispatcher);
  application_handler_.sequenceHandler_.addSequenceObserver(&event_dispatcher);
  application_handler_.sequenceHandler_.addFeaturesObserver(&event_dispatcher);
  event_dispatcher.addTransitionsObserver(&session_handler_);
  event_dispatcher.addSequenceObserver(&session_handler_);
  event_dispatcher.addFeaturesObserver(&session_handler_);
  event_dispatcher.addApplicationProcessorObserver(&layout_loader);
  event_dispatcher.addApplicationProcessorObserver(&application_handler_.session_loader_generator);

  // widgets: pop ups
  auto file_picker_ = std::make_shared<FilePicker>();
  auto session_files_widget_create_ = std::make_shared<SessionFilesWidget>(application_handler_, SessionFilesWidget::Mode::ECreation, workflow_manager_, &event_dispatcher);
  auto session_files_widget_modify_ = std::make_shared<SessionFilesWidget>(application_handler_, SessionFilesWidget::Mode::EModification, workflow_manager_, &event_dispatcher);
  auto create_session_widget_ = std::make_shared<CreateSessionWidget>(application_handler_, session_files_widget_create_);
  auto run_workflow_widget_ = std::make_shared<RunWorkflowWidget>(application_handler_,
    session_handler_,
    workflow_manager_,
    event_dispatcher,
    event_dispatcher,
    event_dispatcher,
    event_dispatcher);
  auto about_widget_ = std::make_shared<AboutWidget>();
  auto options_widget_ = std::make_shared<OptionsWidget>();
  auto report_ = std::make_shared<Report>(application_handler_);

  auto load_session_wizard_ = std::make_shared<LoadSessionWizard>(
    session_files_widget_modify_,
    workflow_manager_,
    application_handler_,
    &event_dispatcher,
    &event_dispatcher,
    &event_dispatcher,
    &event_dispatcher
    );

  // widgets: windows
  auto quickInfoText_= std::make_shared<InfoWidget>("Info", application_handler_,
                                                            event_dispatcher, 
                                                            event_dispatcher,
                                                            event_dispatcher,
                                                            event_dispatcher,
                                                            event_dispatcher);
  auto workflow_ = std::make_shared<WorkflowWidget>("Workflow", application_handler_, workflow_manager_);
  auto statistics_ = std::make_shared<StatisticsWidget>("Statistics", application_handler_, event_dispatcher);
  auto log_widget_ = std::make_shared<LogWidget>(appender_, "Log");
  auto parameters_table_widget_ = std::make_shared<ParametersTableWidget>(session_handler_, application_handler_, "ParametersMainWindow", "Parameters");
  auto chromatogram_plot_widget_ = std::make_shared<ChromatogramPlotWidget>(session_handler_, application_handler_, "Chromatograms Main Window", "Chromatograms", event_dispatcher);
  auto chromatogram_ms2_xic_plot_widget_ = std::make_shared<ChromatogramXICPlotWidget>(session_handler_, application_handler_, "Chromatogram MS2 XIC Main Window", "Chromatograms MS2 XIC", nullptr, event_dispatcher);
  auto spectra_ms2_plot_widget_ = std::make_shared<SpectraMSMSPlotWidget>(session_handler_, application_handler_, "Spectra MS2 Main Window", "Spectra MS2", chromatogram_ms2_xic_plot_widget_, 2, event_dispatcher);
  auto chromatogram_ms1_xic_plot_widget_ = std::make_shared<ChromatogramXICPlotWidget>(session_handler_, application_handler_, "Chromatogram MS1 XIC Main Window", "Chromatograms MS1 XIC", spectra_ms2_plot_widget_, event_dispatcher);
  auto spectra_msms_plot_widget_ = std::make_shared<SpectraMSMSPlotWidget>(session_handler_, application_handler_, "Spectra MS1 Main Window", "Spectra MS1", chromatogram_ms1_xic_plot_widget_, 1, event_dispatcher);
  auto chromatogram_tic_plot_widget_ = std::make_shared<ChromatogramTICPlotWidget>(session_handler_, application_handler_, "Chromatogram TIC Main Window", "Chromatograms TIC", spectra_msms_plot_widget_, event_dispatcher);
  auto heatmap_plot_widget_ = std::make_shared<Heatmap2DWidget>(session_handler_, application_handler_, "Heatmap Main Window", "Features (heatmap)", event_dispatcher);
  auto spectra_plot_widget_ = std::make_shared<SpectraPlotWidget>(session_handler_, application_handler_, "Spectra Main Window", "Spectra", event_dispatcher);
  auto feature_line_plot_ = std::make_shared<LinePlot2DWidget>("Features (line)");
  auto injections_explorer_window_ = std::make_shared<ExplorerWidget>("InjectionsExplorerWindow", "Injections", &event_dispatcher);
  auto calibrators_line_plot_ = std::make_shared<CalibratorsPlotWidget>(
    session_handler_, 
    application_handler_.sequenceHandler_, 
    injections_explorer_window_, 
    chromatogram_plot_widget_,
    event_dispatcher,
    "Calibrators");
  auto transitions_explorer_window_ = std::make_shared<ExplorerWidget>("TransitionsExplorerWindow", "Transitions", &event_dispatcher);
  auto features_explorer_window_ = std::make_shared<ExplorerWidget>("FeaturesExplorerWindow", "Features", &event_dispatcher);
  auto sequence_main_window_ = std::make_shared<SequenceTableWidget>("SequenceMainWindow", "Sequence",
                                                                      &session_handler_, &application_handler_.sequenceHandler_);
  auto transitions_main_window_ = std::make_shared<GenericTableWidget>("TransitionsMainWindow", "Transitions Table");
  auto spectrum_main_window_ = std::make_shared<GenericTableWidget>("SpectrumMainWindow", "Scans Table");
  auto quant_method_main_window_ = std::make_shared<SequenceSegmentWidget>("QuantMethodMainWindow", "Quantitation Method",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setQuantMethodTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto stds_consc_main_window_ = std::make_shared<SequenceSegmentWidget>("StdsConcsMainWindow", "Standards Concentrations",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setStdsConcsTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_filters_main_window_ = std::make_shared<SequenceSegmentWidget>("CompFiltersMainWindow", "Component Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentFiltersTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_group_filters_main_window_ = std::make_shared<SequenceSegmentWidget>("CompGroupFiltersMainWindow", "Component Group Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupFiltersTable, &SessionHandler::getGroupFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_qc_main_window_ = std::make_shared<SequenceSegmentWidget>("CompQCsMainWindow", "Component QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentQCsTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_group_qc_main_window_ = std::make_shared<SequenceSegmentWidget>("CompGroupQCsMainWindow", "Component Group QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupQCsTable, &SessionHandler::getGroupFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_rsd_filters_main_window_ = std::make_shared<SequenceSegmentWidget>("CompRSDFiltersMainWindow", "Component RSD Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentRSDFiltersTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_group_rds_filters_main_window_ = std::make_shared<SequenceSegmentWidget>("CompGroupRSDFiltersMainWindow", "Component Group RSD Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupRSDFiltersTable, &SessionHandler::getGroupFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_rsdcqcs_main_window_ = std::make_shared<SequenceSegmentWidget>("CompRSDQCsMainWindow", "Component RSD QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentRSDQCsTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_group_rsdqcs_main_window_ = std::make_shared<SequenceSegmentWidget>("CompGroupRSDQCsMainWindow", "Component Group RSD QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_, 
                                   &SessionHandler::setComponentGroupRSDQCsTable, &SessionHandler::getGroupFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_background_filters_main_window_ = std::make_shared<SequenceSegmentWidget>("CompBackgroundFiltersMainWindow", "Component Background Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_, 
                                   &SessionHandler::setComponentBackgroundFiltersTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_group_background_filters_main_window_ = std::make_shared<SequenceSegmentWidget>("CompGroupBackgroundFiltersMainWindow", "Component Group Background Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_, 
                                   &SessionHandler::setComponentGroupBackgroundFiltersTable, &SessionHandler::getGroupFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_background_qcs_main_window_ = std::make_shared<SequenceSegmentWidget>("CompBackgroundQCsMainWindow", "Component Background QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_, 
                                   &SessionHandler::setComponentBackgroundQCsTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_group_background_qcs_main_window_ = std::make_shared<SequenceSegmentWidget>("CompGroupBackgroundQCsMainWindow", "Component Group Background QCs",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupBackgroundQCsTable, &SessionHandler::getGroupFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_rsd_estimations_main_window_ = std::make_shared<SequenceSegmentWidget>("CompRSDEstimationsMainWindow", "Component RSD Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentRSDEstimationsTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_group_rsd_estimation_main_window_ = std::make_shared<SequenceSegmentWidget>("CompGroupRSDEstimationsMainWindow", "Component Group RSD Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupRSDEstimationsTable, &SessionHandler::getGroupFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_background_estimations_main_window_ = std::make_shared<SequenceSegmentWidget>("CompBackgroundEstimationsMainWindow", "Component Background Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentBackgroundEstimationsTable, &SessionHandler::getFiltersTable, &application_handler_.sequenceHandler_);
  auto comp_group_background_estimations_main_window_ = std::make_shared<SequenceSegmentWidget>("CompGroupBackgroundEstimationsMainWindow", "Component Group Background Filters",
                                   &session_handler_, &application_handler_.sequenceHandler_,
                                   &SessionHandler::setComponentGroupBackgroundEstimationsTable, &SessionHandler::getGroupFiltersTable, &application_handler_.sequenceHandler_);
  auto features_table_main_window_ = std::make_shared<FeaturesTableWidget>("featuresTableMainWindow", "Features table", &event_dispatcher);
  auto feature_matrix_main_window_ = std::make_shared<GenericTableWidget>("featureMatrixMainWindow", "Features matrix");

  // visible on start
  workflow_->visible_ = true;
  quickInfoText_->visible_ = true;
  injections_explorer_window_->visible_ = true;
  transitions_explorer_window_->visible_ = true;

  // windows organization
  split_window.top_windows_ = {
    statistics_,
    sequence_main_window_,
    transitions_main_window_,
    spectrum_main_window_,
    workflow_,
    parameters_table_widget_,
    quant_method_main_window_,
    stds_consc_main_window_,
    comp_filters_main_window_,
    comp_group_filters_main_window_,
    comp_qc_main_window_,
    comp_group_qc_main_window_,
    comp_rsd_filters_main_window_,
    comp_group_rds_filters_main_window_,
    comp_rsdcqcs_main_window_,
    comp_group_rsdqcs_main_window_,
    comp_background_filters_main_window_,
    comp_group_background_filters_main_window_,
    comp_background_qcs_main_window_,
    comp_group_background_qcs_main_window_,
    comp_rsd_estimations_main_window_,
    comp_group_rsd_estimation_main_window_,
    comp_background_estimations_main_window_,
    comp_group_background_estimations_main_window_,
    features_table_main_window_,
    feature_matrix_main_window_,
    chromatogram_plot_widget_,
    chromatogram_tic_plot_widget_,
    chromatogram_ms1_xic_plot_widget_,
    chromatogram_ms2_xic_plot_widget_,
    spectra_plot_widget_,
    feature_line_plot_,
    heatmap_plot_widget_,
    calibrators_line_plot_
  };

  split_window.bottom_windows_ = {
    quickInfoText_,
    log_widget_,
    spectra_msms_plot_widget_,
    spectra_ms2_plot_widget_,
  };

  split_window.left_windows_ = {
    injections_explorer_window_,
    transitions_explorer_window_,
    features_explorer_window_
  };

  std::vector<std::shared_ptr<Widget>> popups = {
      file_picker_,
      session_files_widget_create_,
      session_files_widget_modify_,
      create_session_widget_,
      run_workflow_widget_,
      options_widget_,
      about_widget_,
      report_,
      load_session_wizard_->set_input_output_widget
  };

  split_window.setupLayoutLoader(layout_loader);

  // We need titles for all sub windows
  checkTitles(split_window.top_windows_);
  checkTitles(split_window.bottom_windows_);
  checkTitles(split_window.left_windows_);

  // Create log path
  const std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char filename[128];
  strftime(filename, 128, "smartpeak_log_%Y-%m-%d_%H-%M-%S.log", std::localtime(&t));

  auto logfilepath = std::filesystem::path{};
  auto logdirpath = std::string{};
  auto logdir_created = false;
  auto error_msg = std::string{};
  try
  {
    std::tie(logfilepath, logdir_created) = Utilities::getLogFilepath(filename);
    logdirpath = std::filesystem::path(logfilepath).parent_path().string();
  }
  catch (const std::runtime_error& re)
  {
    error_msg = re.what();
  }

  // Add .csv appender: 32 MiB per file, max. 100 log files
  plog::RollingFileAppender<plog::CsvFormatter>
    fileAppender(logfilepath.string().c_str(), 1024 * 1024 * 32, 100);

  // Add console appender, instead of only the file one
#ifdef _WIN32
  plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
#else
  plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
#endif
  
  // Init logger with all the appenders
  plog::init(plog::debug, &fileAppender)
    .addAppender(&consoleAppender)
    .addAppender(&appender_);

  // Log SmartPeak launch initiated:
  LOG_INFO << "Start SmartPeak version " << Utilities::getSmartPeakVersion();

  if (error_msg.empty())
  {
    if (logdir_created) { LOG_DEBUG << "Log directory created: " << logdirpath; }
    LOG_INFO << "Log file at: " << logfilepath.string();
  }
  else
  {
    // In this case it will only use console appender
    LOG_WARNING << error_msg;
  }

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Setup window
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  auto starting_window_width = display_mode.w * 0.75;
  auto starting_window_height = display_mode.h * 0.75;
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window* window = SDL_CreateWindow(getMainWindowTitle(application_handler_).c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, starting_window_width, starting_window_height, window_flags);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL2_Init();

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  bool done = false;
  while (!done)
  {
    SDL_SetWindowTitle(window, getMainWindowTitle(application_handler_).c_str());
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
        done = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    split_window.win_size_and_pos_.applyLayout();
    ImGui::NewFrame();

    event_dispatcher.dispatchEvents();

    session_handler_.setMinimalDataAndFilters(application_handler_.sequenceHandler_);

    if ((!workflow_is_done_) && workflow_manager_.isWorkflowDone()) // workflow just finished
    {
      workflow_manager_.updateApplicationHandler(application_handler_);
    }
    workflow_is_done_ = workflow_manager_.isWorkflowDone();
    file_loading_is_done_ = file_picker_->fileLoadingIsDone();

    // Make the quick info text
    quickInfoText_->clearErrorMessages();
    if (exceeding_table_size_) quickInfoText_->addErrorMessage("Table rendering limit reached.  Not showing all selected data.");
    if (ran_integrity_check_ && integrity_check_failed_) quickInfoText_->addErrorMessage("Integrity check failed.  Check the `Information` log.");
    if (ran_integrity_check_ && !integrity_check_failed_) quickInfoText_->addErrorMessage("Integrity check passed.");
      
    // ======================================
    // Popups
    // ======================================
    for (const auto& popup : popups)
    {
      if (popup->visible_)
      {
        if (!ImGui::IsPopupOpen(popup->title_.c_str()))
        {
          ImGui::OpenPopup(popup->title_.c_str());
        }
        popup->draw();
      }
    }

    // ======================================
    // Menu
    // ======================================
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        ImGui::MenuItem("Session", NULL, false, false);
        if (ImGui::MenuItem("New Session", NULL, false, workflow_is_done_ && file_loading_is_done_)) {
          create_session_widget_->visible_ = true;
        }
        if (ImGui::MenuItem("Load Session", NULL, false, workflow_is_done_ && file_loading_is_done_)) {
          file_picker_->open("Select session file",
            load_session_wizard_,
            FilePicker::Mode::EFileRead,
            application_handler_);
        }
        if (ImGui::MenuItem("Save Session",
          NULL,
          false,
          workflow_is_done_ && file_loading_is_done_
          && application_handler_.filenames_.getSessionDB().getDBFilePath() != "")) {
          SaveSession save_session(application_handler_);
          save_session.addApplicationProcessorObserver(&event_dispatcher);
          save_session.process();
        }
        if (ImGui::MenuItem("Save Session As ...", NULL, false, 
                             workflow_is_done_ && file_loading_is_done_ && application_handler_.sessionIsOpened())) {
          auto save_session = std::make_shared<SaveSession>(application_handler_);
          save_session->addApplicationProcessorObserver(&event_dispatcher);
          file_picker_->open("Select session file",
            save_session,
            FilePicker::Mode::EFileCreate,
            application_handler_,
            "session.db");
        }
        if (ImGui::MenuItem("Edit Session Files", NULL, false, application_handler_.sessionIsOpened())) {
          session_files_widget_modify_->open(application_handler_.filenames_);
        }
        if (ImGui::BeginMenu("Export File"))
        {
          // List of tuple: IFilenamesHandler, and a boolean (true for file selection, false for directory selection)
          static std::vector<std::tuple<std::shared_ptr<IFilenamesHandler>, bool>> export_processors =
          {
            {std::make_shared<StoreSequence>(application_handler_.sequenceHandler_), true},
            {std::make_shared<StoreSequenceFileAnalyst>(), true},
            {std::make_shared<StoreSequenceFileMasshunter>(), true},
            {std::make_shared<StoreSequenceFileXcalibur>(), true},
            {std::make_shared<StoreParameters>(), true},
            {std::make_shared<StoreWorkflow>(application_handler_.sequenceHandler_), true},
            {std::make_shared<StoreValidationData>(), true},
            // {std::make_shared<StoreStandardsConcentrations>(), true}, // not implemented
            {std::make_shared<StoreQuantitationMethods>(true), false},
            {std::make_shared<StoreFeatureFilters>(FeatureFiltersUtilsMode::EFeatureFiltersModeComponent, false, true), true},           // have only one copy per sequence.
            {std::make_shared<StoreFeatureFilters>(FeatureFiltersUtilsMode::EFeatureFiltersModeGroup, false, true), true},               // have only one copy per sequence.
            {std::make_shared<StoreFeatureQCs>(FeatureFiltersUtilsMode::EFeatureFiltersModeComponent, false, true), true},               // have only one copy per sequence.
            {std::make_shared<StoreFeatureQCs>(FeatureFiltersUtilsMode::EFeatureFiltersModeGroup, false, true), true},                   // have only one copy per sequence.
            {std::make_shared<StoreFeatureRSDFilters>(FeatureFiltersUtilsMode::EFeatureFiltersModeComponent, false, true), true},        // have only one copy per sequence.
            {std::make_shared<StoreFeatureRSDFilters>(FeatureFiltersUtilsMode::EFeatureFiltersModeGroup, false, true), true},            // have only one copy per sequence.
            {std::make_shared<StoreFeatureRSDQCs>(FeatureFiltersUtilsMode::EFeatureFiltersModeComponent, false, true), true},            // have only one copy per sequence.
            {std::make_shared<StoreFeatureRSDQCs>(FeatureFiltersUtilsMode::EFeatureFiltersModeGroup, false, true), true},                // have only one copy per sequence.
            {std::make_shared<StoreFeatureBackgroundFilters>(FeatureFiltersUtilsMode::EFeatureFiltersModeComponent, false, true), true}, // have only one copy per sequence.
            {std::make_shared<StoreFeatureBackgroundFilters>(FeatureFiltersUtilsMode::EFeatureFiltersModeGroup, false, true), true},     // have only one copy per sequence.
            {std::make_shared<StoreFeatureBackgroundQCs>(FeatureFiltersUtilsMode::EFeatureFiltersModeComponent, false, true), true},     // have only one copy per sequence.
            {std::make_shared<StoreFeatureBackgroundQCs>(FeatureFiltersUtilsMode::EFeatureFiltersModeGroup, false, true), true},         // have only one copy per sequence.
            {std::make_shared<StoreFeatureRSDEstimations>(FeatureFiltersUtilsMode::EFeatureFiltersModeComponent, false, true), false},
            {std::make_shared<StoreFeatureRSDEstimations>(FeatureFiltersUtilsMode::EFeatureFiltersModeGroup, false, true), false},
            {std::make_shared<StoreFeatureBackgroundEstimations>(FeatureFiltersUtilsMode::EFeatureFiltersModeComponent, false, true), false},
            {std::make_shared<StoreFeatureBackgroundEstimations>(FeatureFiltersUtilsMode::EFeatureFiltersModeGroup, false, true), false}
          };
          for (const auto& [export_processor, export_as_file] : export_processors)
          {
            Filenames filenames;
            export_processor->getFilenames(filenames);
            for (const auto& file_id : filenames.getFileIds())
            {
              auto file_picker_handler = std::dynamic_pointer_cast<IFilePickerHandler>(export_processor);
              if (file_picker_handler)
              {
                FilePicker::Mode file_picker_mode = export_as_file ? FilePicker::Mode::EFileCreate : FilePicker::Mode::EDirectory;
                if (ImGui::MenuItem(filenames.getDescription(file_id).c_str(), NULL, false, workflow_is_done_))
                {
                  file_picker_->open(std::string("Export ") + filenames.getDescription(file_id),
                    file_picker_handler,
                    file_picker_mode,
                    application_handler_,
                    filenames.getFullPath(file_id).filename().generic_string()
                  );
                }
              }
            }
          }
          ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit")) {
          done = true;
        }
        ImGui::EndMenu();
        showQuickHelpToolTip("export_file");
      }
      showQuickHelpToolTip("file");
      
      if (ImGui::BeginMenu("View"))
      {
        ImGui::MenuItem("Explorer window", NULL, false, false);
        if (ImGui::MenuItem("Injections", NULL, &injections_explorer_window_->visible_)) {}
        if (ImGui::MenuItem("Transitions", NULL, &transitions_explorer_window_->visible_)) {}
        if (ImGui::MenuItem("Features", NULL, &features_explorer_window_->visible_)) {}
        ImGui::Separator(); // Primary input
        ImGui::MenuItem("Main window (Tables)", NULL, false, false);
        if (ImGui::MenuItem("Sequence", NULL, &sequence_main_window_->visible_)) {}
        if (ImGui::MenuItem("Transitions", NULL, &transitions_main_window_->visible_)) {}
        if (ImGui::MenuItem("Scans", NULL, &spectrum_main_window_->visible_)) {}
        if (ImGui::MenuItem("Workflow", NULL, &workflow_->visible_)) {}
        if (ImGui::BeginMenu("Workflow settings"))
        {
          if (ImGui::MenuItem("Parameters", NULL, &parameters_table_widget_->visible_)) {}
          if (ImGui::MenuItem("Quant Method", NULL, &quant_method_main_window_->visible_)) {}
          if (ImGui::MenuItem("Standards Conc", NULL, &stds_consc_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Filters", NULL, &comp_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group Filters", NULL, &comp_group_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp QCs", NULL, &comp_qc_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group QCs", NULL, &comp_group_qc_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp RSD Filters", NULL, &comp_rsd_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group RSD Filters", NULL, &comp_group_rds_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp RSD QCs", NULL, &comp_rsdcqcs_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group RSD QCs", NULL, &comp_group_rsdqcs_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Background Filters", NULL, &comp_background_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group Background Filters", NULL, &comp_group_background_filters_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Background QCs", NULL, &comp_background_qcs_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group Background QCs", NULL, &comp_group_background_qcs_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp RSD Estimations", NULL, &comp_rsd_estimations_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group RSD Estimations", NULL, &comp_group_rsd_estimation_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Background Estimations", NULL, &comp_background_estimations_main_window_->visible_)) {}
          if (ImGui::MenuItem("Comp Group Background Estimations", NULL, &comp_group_background_estimations_main_window_->visible_)) {}
          // TODO: missing workflow setting tables...
          ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Features (table)", NULL, &features_table_main_window_->visible_)) {}
        if (ImGui::MenuItem("Features (matrix)", NULL, &feature_matrix_main_window_->visible_)) {}
        ImGui::Separator();
        ImGui::MenuItem("Main window (Plots)", NULL, false, false);
        // TODO work on generalization of visualization.
        if (application_handler_.sequenceHandler_.getSequence().size() > 0 && 
            application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getChromatogramMap().getChromatograms().size() > 0)
        {
          if (ImGui::MenuItem("Chromatogram", NULL, &chromatogram_plot_widget_->visible_)) {}
        }
        else
        {
          if (ImGui::MenuItem("Chromatogram", NULL, &chromatogram_tic_plot_widget_->visible_)) {}
        }
        if (ImGui::MenuItem("Spectra", NULL, &spectra_plot_widget_->visible_)) {}
        if (ImGui::MenuItem("Features (line)", NULL, &feature_line_plot_->visible_)) {}
        if (ImGui::MenuItem("Features (heatmap)", NULL, &heatmap_plot_widget_->visible_)) {}
        if (ImGui::MenuItem("Calibrators", NULL, &calibrators_line_plot_->visible_)) {}
        if (ImGui::MenuItem("Statistics", NULL, &statistics_->visible_)) {}
        ImGui::Separator(); 
        ImGui::MenuItem("Info window", NULL, false, false);
        if (ImGui::MenuItem("Info", NULL, &quickInfoText_->visible_)) {}
        if (ImGui::MenuItem("Log", NULL, &log_widget_->visible_)) {}
        ImGui::EndMenu();
      }
      showQuickHelpToolTip("view");
      
      if (ImGui::BeginMenu("Actions"))
      {
        if (ImGui::MenuItem("Run workflow"))
        {
          BuildCommandsFromNames buildCommandsFromNames(application_handler_);
          buildCommandsFromNames.names_ = application_handler_.sequenceHandler_.getWorkflow();
          if (!buildCommandsFromNames.process())
          {
            LOGE << "Failed to create Commands, aborting.";
          }
          else
          {
            auto requirements = workflow_manager_.getRequirements(buildCommandsFromNames.commands_);
            bool missing_requirement = workflow_manager_.isMissingRequirements(application_handler_.filenames_, requirements);
            if (missing_requirement)
            {
              session_files_widget_modify_->open(application_handler_.filenames_, requirements);
            }
            else
            {
              run_workflow_widget_->visible_ = true;
              if (application_handler_.sequenceHandler_.getWorkflow().empty())
              {
                LOGW << "Workflow has no steps to run. Please set the workflow's steps.";
              }
              initializeDataDirs(application_handler_);
            }
          }
        }
        showQuickHelpToolTip("run_workflow");
        
        if (ImGui::MenuItem("Cancel Workflow"))
        {
          if (run_on_server) {
            if (workflow_client_.isChannelSet()) {
              workflow_client_.stopRunningWorkflow();
            }
          }
        }
        
        if (ImGui::BeginMenu("Integrity checks"))
        {
          if (ImGui::MenuItem("Sample consistency")) {
            ran_integrity_check_ = true;
            integrity_check_failed_ = !InputDataValidation::sampleNamesAreConsistent(application_handler_.sequenceHandler_);
          }
          if (ImGui::MenuItem("Comp consistency")) {
            ran_integrity_check_ = true;
            integrity_check_failed_ = !InputDataValidation::componentNamesAreConsistent(application_handler_.sequenceHandler_);
          }
          if (ImGui::MenuItem("Comp Group consistency")) {
            ran_integrity_check_ = true;
            integrity_check_failed_ = !InputDataValidation::componentNameGroupsAreConsistent(application_handler_.sequenceHandler_);
          }
          if (ImGui::MenuItem("IS consistency")) {
            ran_integrity_check_ = true;
            integrity_check_failed_ = !InputDataValidation::heavyComponentsAreConsistent(application_handler_.sequenceHandler_);
          }
          ImGui::EndMenu();
        }
        showQuickHelpToolTip("integrity_checks");
        if (ImGui::MenuItem("Report"))
        {
          report_->visible_ = true;
        }
        showQuickHelpToolTip("report");
        ImGui::EndMenu();
      }
      showQuickHelpToolTip("actions");
      
      if (ImGui::BeginMenu("Window"))
      {
        if (ImGui::MenuItem("Reset Window Layout"))
        {
          split_window.reset_layout_ = true;
          calibrators_line_plot_->reset_layout_ = true;
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Tools"))
      {
        ImGui::MenuItem("Options", NULL, &options_widget_->visible_);
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Help"))
      {
        ImGui::MenuItem("About", NULL, &about_widget_->visible_);
        ImGui::MenuItem("Show Quick Help", NULL, &enable_quick_help);
        if (ImGui::MenuItem("Documentation")) {
          // TODO: Render the SmartPeak documentation (See AUT-178)
        }
        ImGui::EndMenu();
      }
      showQuickHelpToolTip("help");
      
      ImGui::EndMainMenuBar();
    }
      
    // ======================================
    // Server-Side Execution
    // ======================================
      if (run_workflow_widget_->server_fields_set)
      {
        if (run_workflow_widget_->run_remote_workflow)
        {
          workflow_client_.setChannel(grpc::CreateChannel(
            run_workflow_widget_->server_url,
            grpc::InsecureChannelCredentials()));

          runworkflow_future_ = std::async(
            std::launch::async,
            &WorkflowClient::runWorkflow,
            &workflow_client_,
            application_handler_.filenames_.getTagValue(Filenames::Tag::MAIN_DIR),
            run_workflow_widget_->username,
            Utilities::sha256(run_workflow_widget_->password)
          );
        }
        
        run_workflow_widget_->server_fields_set = false;
        RawDataAndFeatures_loaded_ = false;
      }
      
      if (run_workflow_widget_->run_remote_workflow)
      {
        if (runworkflow_future_.valid())
        {
          workflow_client_.getEvent(
            &event_dispatcher, &event_dispatcher, &event_dispatcher,
            &event_dispatcher, &event_dispatcher, &event_dispatcher);
          workflow_client_.getLogstream();
          
          ::serv::processRemoteWorkflow(
            runworkflow_future_, run_workflow_widget_->username,
            application_handler_, session_handler_, workflow_manager_,
            event_dispatcher, RawDataAndFeatures_loaded_);
        }
      }
    // ======================================
    // Data updates
    //
    // (Widgets should update their data 
    // themself, which is not the case for
    // all of them yet)
    // ======================================
    
    // Sequence
    if (sequence_main_window_->visible_)
    {
      sequence_main_window_->checked_rows_ = session_handler_.getSequenceTableFilters();
      sequence_main_window_->table_data_ = session_handler_.sequence_table;
    }

    // Transitions
    if (transitions_main_window_->visible_)
    {
      transitions_main_window_->checked_rows_ = session_handler_.getTransitionsTableFilters();
      transitions_main_window_->table_data_ = session_handler_.transitions_table;
    }

    // spectrum
    if (spectrum_main_window_->visible_)
    {
      spectrum_main_window_->checked_rows_ = session_handler_.getSpectrumTableFilters();
      spectrum_main_window_->table_data_ = session_handler_.spectrum_table;
    }

    // feature table
    if (features_table_main_window_->visible_)
    {
      exceeding_table_size_ = !session_handler_.setFeatureTable(application_handler_.sequenceHandler_,
                                                                features_table_main_window_->table_data_);
      features_table_main_window_->checked_rows_ = Eigen::Tensor<bool, 1>();
    }

    // feature matrix
    if (feature_matrix_main_window_->visible_)
    {
        session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
        feature_matrix_main_window_->table_data_ = session_handler_.feature_pivot_table;
        feature_matrix_main_window_->checked_rows_ = Eigen::Tensor<bool, 1>();
    }

    // feature line plot
    if (feature_line_plot_->visible_)
    {
      session_handler_.setFeatureMatrix(application_handler_.sequenceHandler_);
      Eigen::Tensor<float, 2> x_data = session_handler_.feat_sample_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
      Eigen::Tensor<float, 2> y_data = session_handler_.feat_value_data.shuffle(Eigen::array<Eigen::Index, 2>({ 1,0 }));
      feature_line_plot_->setValues(x_data, y_data, &session_handler_.feat_col_labels, &session_handler_.feat_row_labels, session_handler_.feat_line_x_axis_title, session_handler_.feat_line_y_axis_title,
        session_handler_.feat_line_sample_min, session_handler_.feat_line_sample_max, session_handler_.feat_value_min, session_handler_.feat_value_max,
        "FeaturesLineMainWindow");
    }

    //statistics
    if (statistics_->visible_)
    {
      statistics_->setInjections(session_handler_.injection_explorer_data.checkbox_body, session_handler_.getInjectionExplorerBody());
      statistics_->setTransitions(&session_handler_.transitions_table.body_, session_handler_.transition_explorer_data.checkbox_body, session_handler_.getTransitionExplorerBody());
    }

    // info
    if (quickInfoText_->visible_)
    {
      quickInfoText_->setTransitions(&session_handler_.transitions_table.body_);
    }

    // injections 
    if (injections_explorer_window_->visible_)
    {
      injections_explorer_window_->table_data_.headers_ = session_handler_.getInjectionExplorerHeader();
      injections_explorer_window_->table_data_.body_ = session_handler_.getInjectionExplorerBody();
      injections_explorer_window_->checked_rows_ = session_handler_.injection_explorer_data.checked_rows;
      injections_explorer_window_->checkbox_headers_ = session_handler_.injection_explorer_data.checkbox_headers;
      injections_explorer_window_->checkbox_columns_ = &session_handler_.injection_explorer_data.checkbox_body;
    }

    // transitions
    if (transitions_explorer_window_->visible_)
    {
      transitions_explorer_window_->table_data_.headers_ = session_handler_.getTransitionExplorerHeader();
      transitions_explorer_window_->table_data_.body_ = session_handler_.getTransitionExplorerBody();
      transitions_explorer_window_->checked_rows_ = session_handler_.transition_explorer_data.checked_rows;
      transitions_explorer_window_->checkbox_headers_ = session_handler_.transition_explorer_data.checkbox_headers;
      transitions_explorer_window_->checkbox_columns_ = &session_handler_.transition_explorer_data.checkbox_body;
    }

    //features
    if (features_explorer_window_->visible_)
    {
      session_handler_.setFeatureExplorer();
      features_explorer_window_->table_data_ = session_handler_.feature_table;
      features_explorer_window_->checked_rows_ = session_handler_.feature_explorer_data.checked_rows;
      features_explorer_window_->checkbox_headers_ = session_handler_.feature_explorer_data.checkbox_headers;
      features_explorer_window_->checkbox_columns_ = &session_handler_.feature_explorer_data.checkbox_body;
    }

    // ======================================
    // Windows display
    // ======================================
    split_window.draw();

    // =====================================================
    // Load/Save Layout (must be call after ui has been set)
    // =====================================================
    layout_loader.process();

    // =====================================================
    // Rendering
    // =====================================================
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
      SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void initializeDataDirs(ApplicationHandler& application_handler)
{
  initializeDataDir(application_handler, "mzML", Filenames::Tag::MZML_INPUT_PATH, "mzML");
  initializeDataDir(application_handler, "INPUT features", Filenames::Tag::FEATURES_INPUT_PATH, "features");
  initializeDataDir(application_handler, "OUTPUT features", Filenames::Tag::FEATURES_OUTPUT_PATH, "features");
}

void initializeDataDir(
  ApplicationHandler& application_handler,
  const std::string& label,
  Filenames::Tag tag,
  const std::filesystem::path& default_dir
)
{
  auto tag_value = application_handler.filenames_.getTagValue(tag);
  if (!tag_value.empty()) {
    return;
  }
  application_handler.filenames_.setTagValue(tag, (application_handler.main_dir_ / default_dir).generic_string());
}

void checkTitles(const std::vector<std::shared_ptr<Widget>> windows)
{
  for (const auto& window : windows)
  {
    assert(window->title_ != std::string(""));
  }
}

std::string getMainWindowTitle(const ApplicationHandler& application_handler)
{
  std::ostringstream os;
  static auto smartpeak_version = static_cast<std::ostringstream&&>(
    std::ostringstream() << "SmartPeak v" << Utilities::getSmartPeakVersion()).str();
  os << smartpeak_version;
  if (!application_handler.sessionIsSaved())
  {
    os << " (unsaved session)";
  }
  return os.str();
}

