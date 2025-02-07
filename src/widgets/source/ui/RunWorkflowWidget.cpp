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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/RunWorkflowWidget.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <plog/Log.h>
#include <filesystem>

namespace SmartPeak
{
  void RunWorkflowWidget::draw()
  {
		ImGui::SetNextWindowSizeConstraints(ImVec2(320, 100), ImVec2(FLT_MAX, FLT_MAX));
    if (ImGui::BeginPopupModal(title_.c_str(), NULL, ImGuiWindowFlags_None))
    {
      if (   mzML_dir_old_ != application_handler_.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH)
          || features_in_dir_old_ != application_handler_.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH)
          || features_out_dir_old_ != application_handler_.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH))
      {
        // InputTextWithHint does not support std::filesystem::path so we have to create temporary strings for the paths
        mzML_dir_old_ = mzML_dir_edit_ = application_handler_.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH);
        features_in_dir_old_ = features_in_dir_edit_ = application_handler_.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH);
        features_out_dir_old_ = features_out_dir_edit_ = application_handler_.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH);
      }

      ImGui::Text("mzML folder");
      ImGui::PushID(1);
      ImGui::InputTextWithHint("", mzML_dir_edit_.c_str(), &mzML_dir_edit_);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(11);
      if (ImGui::Button("Select"))
      {
        popup_file_picker_ = std::make_tuple("mzML folder", std::make_shared<SetRawDataPathname>(&application_handler_.filenames_));
      }
      ImGui::PopID();

      ImGui::Text("Input features folder");
      ImGui::PushID(2);
      ImGui::InputTextWithHint("", features_in_dir_edit_.c_str(), &features_in_dir_edit_);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(22);
      if (ImGui::Button("Select"))
      {
        popup_file_picker_ = std::make_tuple("Input features folder", std::make_shared<SetInputFeaturesPathname>(&application_handler_.filenames_));
      }
      ImGui::PopID();

      ImGui::Text("Output features folder");
      ImGui::PushID(3);
      ImGui::InputTextWithHint("", features_out_dir_edit_.c_str(), &features_out_dir_edit_);
      ImGui::PopID();

      ImGui::SameLine();
      ImGui::PushID(33);
      if (ImGui::Button("Select"))
      {
        popup_file_picker_ = std::make_tuple("Output features folder", std::make_shared<SetOutputFeaturesPathname>(&application_handler_.filenames_));
      }
      ImGui::PopID();

      if (popup_file_picker_)
      {
        const auto& [title, file_handler] = (*popup_file_picker_);
        file_picker_.open(title, file_handler, FilePicker::Mode::EDirectory, application_handler_);
        popup_file_picker_ = std::nullopt;
      }

      file_picker_.draw();
      
      ImGui::Separator();
      static int number_of_threads = std::thread::hardware_concurrency();
      if (number_of_threads < 1) number_of_threads = 1;
      ImGui::InputInt("Number of Threads", &number_of_threads);

      ImGui::Separator();
      ImGui::Checkbox("Run on Server", &run_on_server);
      ImGui::Text("Server URL");
      ImGui::SameLine();
      ImGui::PushID(4);
      ImGui::InputTextWithHint("", "localhost:50051", &server_url);
      ImGui::PopID();
      
      if (ImGui::TreeNode("Sign In"))
      {
        ImGui::Text("Please enter your username and password");
        ImGui::PushID(5);
        ImGui::InputTextWithHint("", "username", &username);
        ImGui::PopID();
        ImGui::PushID(6);
        ImGui::InputTextWithHint("", "password", &password, ImGuiInputTextFlags_Password);
        ImGui::PopID();
        ImGui::TreePop();
      }
      
      ImGui::Separator();
      bool run_workflow_clicked = false;
      if ((run_workflow_clicked = ImGui::Button("Run workflow")) && !run_on_server)
      {
        application_handler_.filenames_.setTagValue(Filenames::Tag::MZML_INPUT_PATH, mzML_dir_edit_);
        application_handler_.filenames_.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, features_in_dir_edit_);
        application_handler_.filenames_.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, features_out_dir_edit_);
        if (checkDirectories())
        {
          BuildCommandsFromNames buildCommandsFromNames(application_handler_);
          buildCommandsFromNames.names_ = application_handler_.sequenceHandler_.getWorkflow();
          if (!buildCommandsFromNames.process()) {
            LOGE << "Failed to create Commands, aborting.";
          }
          else
          {
            for (auto& cmd : buildCommandsFromNames.commands_)
            {
              for (auto& p : cmd.dynamic_filenames)
              {
                p.second.setTagValue(Filenames::Tag::MZML_INPUT_PATH, mzML_dir_edit_);
                p.second.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, features_in_dir_edit_);
                p.second.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, features_out_dir_edit_);
              }
            }
            const std::set<std::string> injection_names = session_handler_.getSelectInjectionNamesWorkflow(application_handler_.sequenceHandler_);
            const std::set<std::string> sequence_segment_names = session_handler_.getSelectSequenceSegmentNamesWorkflow(application_handler_.sequenceHandler_);
            const std::set<std::string> sample_group_names = session_handler_.getSelectSampleGroupNamesWorkflow(application_handler_.sequenceHandler_);
            workflow_manager_.addWorkflow(application_handler_,
              injection_names,
              sequence_segment_names,
              sample_group_names,
              buildCommandsFromNames.commands_,
              number_of_threads,
              &application_processor_observer_,
              &sequence_processor_observer_,
              &sequence_segment_processor_observer_,
              &sample_group_processor_observer_
            );
          }
        }
        visible_ = false;
        ImGui::CloseCurrentPopup();
      }
      if (run_workflow_clicked && !server_url.empty() && !username.empty() && run_on_server)
      {
        application_handler_.filenames_.setTagValue(Filenames::Tag::MZML_INPUT_PATH, mzML_dir_edit_);
        application_handler_.filenames_.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, features_in_dir_edit_);
        application_handler_.filenames_.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, features_out_dir_edit_);
        
        Utilities::createServerSessionFile(application_handler_.filenames_.getTagValue(Filenames::Tag::MAIN_DIR));
        show_serversession_popup_ = Utilities::checkLastServerWorkflowRun(application_handler_.filenames_.getTagValue(Filenames::Tag::MAIN_DIR), username);
        server_fields_set = true;
        run_remote_workflow = true;
        visible_ = false;
        ImGui::CloseCurrentPopup();
      }
      if (show_serversession_popup_)
      {
        ImGui::OpenPopup("Server Session Found");
        if (ImGui::BeginPopupModal("Server Session Found", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
          ImGui::Text("Do you want to load the workflow results from the last run?\n\n");
          ImGui::Separator();

          if (ImGui::Button("Yes", ImVec2(120, 0))) {
            Utilities::loadRawDataAndFeatures(application_handler_, session_handler_, workflow_manager_, event_dispatcher_);
            run_remote_workflow = false;
            show_serversession_popup_ = false;
            visible_ = false;
            ImGui::CloseCurrentPopup();
          }
          ImGui::SetItemDefaultFocus();
          ImGui::SameLine();
          if (ImGui::Button("No - Rerun workflow", ImVec2(160, 0))) {
            run_remote_workflow = true;
            server_fields_set = true;
            show_serversession_popup_ = false;
            visible_ = false;
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
        }
      }
      
      ImGui::SameLine();
      if (ImGui::Button("Close"))
      {
        visible_ = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  bool RunWorkflowWidget::checkDirectories() const
  {
    for (const auto& pathname : { features_out_dir_edit_ })
    {
      if (!std::filesystem::exists(pathname))
      {
        try
        {
          std::filesystem::create_directories(std::filesystem::path(pathname));
        }
        catch (std::filesystem::filesystem_error& fe)
        {
          std::ostringstream os;
          os << "Unable to create Output Directory '" << pathname << "': " << fe.what();
          LOGE << os.str();
          application_processor_observer_.onApplicationProcessorError(os.str());
          return false;
        }
      }
    }
    for (const auto& pathname : { mzML_dir_edit_, features_in_dir_edit_ })
    {
      if (!std::filesystem::exists(pathname))
      {
        std::ostringstream os;
        os << "Input Directory '" << pathname << "' not found, aborting.";
        LOGE << os.str();
        application_processor_observer_.onApplicationProcessorError(os.str());
        return false;
      }
    }
    return true;
  }
}
