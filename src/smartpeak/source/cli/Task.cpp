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
// $Maintainer: Krzysztof Abram $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------
#include <SmartPeak/cli/Task.h>

#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>

#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <SmartPeak/core/ApplicationProcessors/LoadSession.h>

#include <filesystem>


namespace SmartPeak {
  namespace cli {

    bool InitializeApplicationSettings::operator() (ApplicationManager& application_manager)
    {
      auto& application_settings = application_manager.get_application_settings();
      try
      {
        application_settings.define_options();
        application_settings.load_options();
        application_settings.process_options();
        application_settings.validate();
      }
      catch (const std::invalid_argument& ia)
      {
        std::cerr << ia.what() << std::endl;
        std::cerr << application_settings.usage() << std::endl;
        return false;
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
        return false;
      }
      return true;
    }

    bool InitializeLogger::operator() (ApplicationManager& application_manager)
    {
      auto& application_settings = application_manager.get_application_settings();
      try
      {
        auto& console_handler = ConsoleHandler::get_instance();
        console_handler.set_log_directory(application_settings.log_dir);
        console_handler.use_colors(!application_settings.disable_colors);
        console_handler.set_severity(application_settings.verbose ? plog::debug : plog::info);
        console_handler.initialize("Start SmartPeak version " + SmartPeak::Utilities::getSmartPeakVersion());
        if (application_settings.verbose) { LOG_DEBUG << "Verbose mode ON"; }
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
        return false;
      }
      return true;
    }

    bool LoadSession::operator() (ApplicationManager& application_manager)
    {
      auto& application_settings = application_manager.get_application_settings();
      auto& application_handler = application_manager.get_application_handler();
      if (!std::filesystem::exists(application_settings.load_session))
      {
        LOGE << "Session not found: " << application_settings.load_session;
        return false;
      }
      else
      {
        Filenames filenames_override;
        for (const auto& input_file : application_settings.input_files)
        {
          auto key_value = application_settings.get_key_value_from_option(input_file);
          const std::string& file_id = key_value.first;
          const std::filesystem::path filename = key_value.second;
          if (filename.is_relative())
          {
            filenames_override.addFileName(file_id, std::filesystem::path("${MAIN_DIR}/" + filename.generic_string()).lexically_normal().generic_string());
          }
          else
          {
            filenames_override.addFileName(file_id, filename.generic_string());
          }
        }
        
        ParameterSet parameters_override;
        for (const auto& parameter : application_settings.parameters)
        {
          const auto key_value = application_settings.get_key_value_from_option(parameter);
          const auto& key = key_value.first; // key must be FunctionName:ParameterName
          auto separator_pos = key.find(":");
          if (separator_pos != key.npos)
          {
            std::string function_name = key.substr(0, separator_pos);
            std::string parameter_name = key.substr((separator_pos + 1), key.size() - (separator_pos + 1));
            if (!function_name.empty() && !parameter_name.empty())
            {
              std::map<std::string, std::string> param_struct = {
                {"name", parameter_name},
                { "value", key_value.second }
              };
              Parameter param(param_struct);
              parameters_override.addParameter(function_name, param);
            }
          }
        }

        WorkflowManager workflow_manager;
        SmartPeak::LoadSession create_sequence(application_handler, workflow_manager);
        create_sequence.filenames_override_ = filenames_override;
        create_sequence.parameters_override_ = parameters_override;
        if (application_settings.workflow.size())
        {
          create_sequence.workflow_override_ = application_settings.workflow;
        }
        if (std::filesystem::is_regular_file(application_settings.load_session))
        {
          // Load from session
          return create_sequence.onFilePicked(application_settings.load_session, &application_handler);
        }
        else
        {
          LOGW << "Loading session from directory - backward compatibility - deprecated, prefer loading from session file.";
          Filenames filenames_main = Utilities::buildFilenamesFromDirectory(application_handler, application_settings.load_session);
          application_handler.main_dir_ = filenames_main.getTagValue(Filenames::Tag::MAIN_DIR);
          create_sequence.filenames_ = filenames_main;
          return create_sequence.process();
        }
      }
    }

    bool RunIntegrityChecks::operator() (ApplicationManager& application_manager)
    {
      auto& application_settings = application_manager.get_application_settings();
      auto& application_handler = application_manager.get_application_handler();
      try
      {
        auto& integrity_args = application_settings.integrity;
        auto int_msg = std::string{ "Detected integrity option: " };
        auto no_integrity_check = ApplicationSettings::contains_option(integrity_args, "NONE", int_msg);
        auto all_integrity_check = ApplicationSettings::contains_option(integrity_args, "ALL", int_msg);

        if (no_integrity_check)
        {
          LOG_WARNING << "No integrity check are run, continue with the workflow";
        }
        else
        {
          if (all_integrity_check || ApplicationSettings::contains_option(integrity_args, "SAMPLE", int_msg))
          {
            handle_integrity_check(
              run_integrity_check(
                application_handler.sequenceHandler_,
                SmartPeak::InputDataValidation::sampleNamesAreConsistent,
                "Run sample integrity check"),
              application_settings.allow_inconsistent,
              "Sample integrity check failed!");
          }
          if (all_integrity_check || ApplicationSettings::contains_option(integrity_args, "COMP", int_msg))
          {
            handle_integrity_check(
              run_integrity_check(
                application_handler.sequenceHandler_,
                SmartPeak::InputDataValidation::componentNamesAreConsistent,
                "Run compound integrity check"),
              application_settings.allow_inconsistent,
              "Compound integrity check failed!");
          }
          if (all_integrity_check || ApplicationSettings::contains_option(integrity_args, "COMP_GROUP", int_msg))
          {
            handle_integrity_check(
              run_integrity_check(
                application_handler.sequenceHandler_,
                SmartPeak::InputDataValidation::componentNameGroupsAreConsistent,
                "Run compound group integrity check"),
              application_settings.allow_inconsistent,
              "Compound group integrity check failed!");
          }
          if (all_integrity_check || ApplicationSettings::contains_option(integrity_args, "IS", int_msg))
          {
            handle_integrity_check(
              run_integrity_check(
                application_handler.sequenceHandler_,
                SmartPeak::InputDataValidation::heavyComponentsAreConsistent,
                "Run IS integrity check"),
              application_settings.allow_inconsistent,
              "IS integrity check failed!");
          }
        }
      }
      catch (std::runtime_error& re)
      {
        LOG_ERROR << re.what();
        return false;
      }
      return true;
    }

    void RunIntegrityChecks::handle_integrity_check(
      bool check, bool allow_inconsistent, std::string msg)
    {
      // Fail the pipeline if integrity check fails and allow-inconsistent is false:   
      if (!check && !allow_inconsistent)
      {
        throw std::runtime_error(msg);
      }
      // Allow to continue the pipeline if integrity check fails and allow-inconsistent is true: 
      else if (!check && allow_inconsistent)
      {
        LOG_WARNING << msg << " Continue the workflow with --allow-inconsistent enabled.";
      }
      if (check)
      {
        LOG_INFO << "Integrity check succeeded";
      }
    }

    bool RunIntegrityChecks::run_integrity_check(
      const SmartPeak::SequenceHandler& sequence_handler,
      std::function<bool(const SmartPeak::SequenceHandler&)> f,
      const std::string& info_msg)
    {
      LOG_INFO << info_msg;
      return f(sequence_handler);
    }

    bool InitializeWorkflowResources::operator() (ApplicationManager& application_manager)
    {
      namespace fs = std::filesystem;
      auto& application_settings = application_manager.get_application_settings();
      auto& application_handler = application_manager.get_application_handler();

      // Initialize directories:

      std::filesystem::path mzml_dir = application_settings.mzml_dir;
      if (mzml_dir.is_relative())
      {
        mzml_dir = (application_handler.main_dir_ / mzml_dir).lexically_normal();
      }
      application_handler.filenames_.setTagValue(Filenames::Tag::MZML_INPUT_PATH, mzml_dir.generic_string());

      std::filesystem::path features_out_dir = application_settings.features_out_dir;
      if (features_out_dir.is_relative())
      {
        features_out_dir = (application_handler.main_dir_ / features_out_dir).lexically_normal();
      }
      application_handler.filenames_.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, features_out_dir.generic_string());

      std::filesystem::path features_in_dir = application_settings.features_in_dir;
      if (features_in_dir.is_relative())
      {
        features_in_dir = (application_handler.main_dir_ / features_in_dir).lexically_normal();
      }
      application_handler.filenames_.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, features_in_dir.generic_string());

      auto paths = {
          application_handler.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH),
          application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH),
          application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH)
      };
      auto current_path = std::filesystem::path{};
      try
      {
        for (const auto& pathname : paths)
        {
          current_path = pathname;
          fs::create_directories(fs::path(pathname));
        }
      }
      catch (fs::filesystem_error& fe)
      {
        if (fe.code() == std::errc::permission_denied)
          LOG_ERROR << static_cast<std::ostringstream&&>(
            std::ostringstream()
            << "Unable to create output directory, permission denied: '" << current_path << "'").str();
        else
          LOG_ERROR << static_cast<std::ostringstream&&>(
            std::ostringstream()
            << "Unable to create output directory: '" << current_path << "'").str();
        return false;
      }
      return true;
    }

    bool InitializeWorkflowSettings::operator() (ApplicationManager& application_manager)
    {
      auto& application_settings = application_manager.get_application_settings();
      auto& application_handler = application_manager.get_application_handler();
      BuildCommandsFromNames buildCommandsFromNames{ application_handler };
      {
        buildCommandsFromNames.names_ = application_handler.sequenceHandler_.getWorkflow();
        if (!application_settings.workflow.empty())
        {
          // Override workflow given by cli option:
          buildCommandsFromNames.names_ = application_settings.workflow;
          auto workflow_string = std::string{};
          for (const auto& cmd : buildCommandsFromNames.names_) workflow_string += "\t" + cmd + " \n";
          LOG_WARNING << "Override workflow with: \n" << workflow_string;
        }
      }
      if (!buildCommandsFromNames.process())
      {
        LOG_ERROR << "Failed to create workflow commands, abort.";
        return false;
      }
      application_manager.set_workflow_commands(buildCommandsFromNames.commands_);
      _update_filenames(buildCommandsFromNames, application_handler);
      return true;
    }

    void InitializeWorkflowSettings::_update_filenames(
      SmartPeak::BuildCommandsFromNames& buildCommandsFromNames,
      SmartPeak::ApplicationHandler& application_handler)
    {
      for (auto& cmd : buildCommandsFromNames.commands_)
      {
        for (auto& p : cmd.dynamic_filenames)
        {
          p.second.setTagValue(Filenames::Tag::MAIN_DIR, application_handler.main_dir_.generic_string());
          p.second.setTagValue(Filenames::Tag::MZML_INPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::MZML_INPUT_PATH));
          p.second.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_INPUT_PATH));
          p.second.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, application_handler.filenames_.getTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH));
        }
      }
    }

    bool RunWorkflow::operator() (ApplicationManager& application_manager)
    {
      auto& application_settings = application_manager.get_application_settings();
      auto& application_handler = application_manager.get_application_handler();
      auto& session_handler = application_manager.get_session_handler();
      auto& workflow_manager = application_manager.get_workflow_manager();
      auto& event_dispatcher = application_manager.get_event_dispatcher();
      auto& progress_info = application_manager.get_progress_info();
      {
        event_dispatcher.addApplicationProcessorObserver(this);
        event_dispatcher.addSequenceProcessorObserver(this);
        event_dispatcher.addSequenceSegmentProcessorObserver(this);
        event_dispatcher.addSampleGroupProcessorObserver(this);
      }
      try
      {
        const auto injection_names = session_handler.getSelectInjectionNamesWorkflow(
          application_handler.sequenceHandler_);
        const auto sequence_segment_names = session_handler.getSelectSequenceSegmentNamesWorkflow(
          application_handler.sequenceHandler_);
        const auto sample_group_names = session_handler.getSelectSampleGroupNamesWorkflow(
          application_handler.sequenceHandler_);

        // If this flag is true, no progressbar is printed and workflow is ran on the main thread.
        auto disable_progressbar = application_settings.disable_progressbar;

        int number_of_threads = application_settings.nb_threads;
        if (number_of_threads < 1)
        {
          number_of_threads = std::thread::hardware_concurrency();
        }
        workflow_manager.addWorkflow(
          application_handler,
          injection_names,
          sequence_segment_names,
          sample_group_names,
          application_manager.get_workflow_commands(),
          number_of_threads,
          &event_dispatcher,
          &event_dispatcher,
          &event_dispatcher,
          &event_dispatcher,
          disable_progressbar);

        if (!disable_progressbar)
        {
          auto freq = static_cast<double>(20.); // Hz
          auto rate = std::chrono::milliseconds(static_cast<uint64_t>(1000. / freq));
          while (!workflow_manager.isWorkflowDone())
          {
            event_dispatcher.dispatchEvents();
            std::this_thread::sleep_for(rate);
            show_progress(progress_info, 40);
          }
        }
      }
      catch (const std::exception& e)
      {
        LOG_ERROR << e.what();
        return false;
      }
      return true;
    }

    std::string RunWorkflow::formatted_time(const std::chrono::steady_clock::duration& duration) const
    {
      std::ostringstream os;
      auto ns = duration;
      auto h = std::chrono::duration_cast<std::chrono::hours>(ns);
      ns -= h;
      auto m = std::chrono::duration_cast<std::chrono::minutes>(ns);
      ns -= m;
      auto s = std::chrono::duration_cast<std::chrono::seconds>(ns);
      os << std::setfill('0') << std::setw(2) << h.count() << "h:"
        << std::setw(2) << m.count() << "m:"
        << std::setw(2) << s.count() << 's';
      return os.str();
    }

    void RunWorkflow::show_progress(const ProgressInfo& progress_info, int bar_width) const
    {
      auto running_time = progress_info.runningTime();
      auto progress = progress_info.progressValue();
      auto estimated_time = progress_info.estimatedRemainingTime();

      auto poss = std::ostringstream{};
      auto eta = std::string{};
      if (estimated_time)
      {
        if ((*estimated_time).count() < 0)
        {
          (*estimated_time) = std::chrono::steady_clock::duration::zero();
        }
        eta = formatted_time(*estimated_time);
      }
      else
      {
        eta = "N/A";
      }

      auto et = std::string{};
      if (0 == m_event_type) et = " Seq";
      else if (1 == m_event_type) et = "SeqS";
      else if (2 == m_event_type) et = "SeqG";

      poss << " " << formatted_time(running_time) << " " << "[" << et << ": " << m_event_name << "]" << " [";
      int pos = bar_width * progress;
      for (int i = 0; i < bar_width; ++i) {
        if (i < pos) poss << "=";
        else if (i == pos) poss << ">";
        else poss << " ";
      }
      poss << "] " << int(progress * 100.0) << "%";
      poss << " ETA " << eta << "\r";

      std::cout << poss.str();
      std::cout.flush();
    }

    bool ExportReport::operator() (ApplicationManager& application_manager)
    {
      auto& application_settings = application_manager.get_application_settings();
      auto& application_handler = application_manager.get_application_handler();
      try
      {
        auto all_reports = ApplicationSettings::contains_option(
          application_settings.report, "ALL", "Detected report option: ");
        auto feature_db = all_reports || ApplicationSettings::contains_option(
          application_settings.report, "FEATUREDB", "Detected report option: ");
        auto pivot_table = all_reports || ApplicationSettings::contains_option(
          application_settings.report, "PIVOTTABLE", "Detected report option: ");

        auto report_sample_types = std::set<SampleType>{};
        auto report_metadata = std::vector<FeatureMetadata>{};
        _extract_report_sampletypes(application_settings, report_sample_types);
        _extract_report_metadata(application_settings, report_metadata);

        std::filesystem::path reports_out_dir = application_settings.reports_out_dir;
        if (reports_out_dir.is_relative())
        {
          reports_out_dir = (application_handler.main_dir_ / reports_out_dir).lexically_normal();
        }
        if (!std::filesystem::exists(std::filesystem::path(reports_out_dir)) &&
            !std::filesystem::create_directories(std::filesystem::path(reports_out_dir)))
        {
          LOGE << "Failed to create output report directory: " << reports_out_dir.generic_string();
        }

        if (feature_db)
        {
          auto& sequance_handler = application_handler.sequenceHandler_;
          const auto filepath = reports_out_dir / "FeatureDB.csv";
          SequenceParser::writeDataTableFromMetaValue(
            sequance_handler, filepath,
            report_metadata, report_sample_types);
        }
        if (pivot_table)
        {
          auto& sequance_handler = application_handler.sequenceHandler_;
          const auto filepath = reports_out_dir / "PivotTable.csv";
          SequenceParser::writeDataMatrixFromMetaValue(
            sequance_handler, filepath,
            report_metadata, report_sample_types);
        }
      }
      catch (const std::exception& e)
      {
        LOG_ERROR << e.what();
        return false;
      }
      return true;
    }

    void ExportReport::_extract_report_sampletypes(
      const ApplicationSettings& application_settings,
      std::set<SampleType>& report_sample_types)
    {
      auto& available_types = SmartPeak::sampleTypeToString;
      auto& types = application_settings.report_sample_types;
      if (ApplicationSettings::contains_option(
        types, "ALL", "Detected report-sample-types option: "))
      {
        // If no sample-types options are selected, use all features:
        std::transform(available_types.cbegin(), available_types.cend(),
          std::inserter(report_sample_types, report_sample_types.begin()),
          [](const auto& m) { return m.first; });
      }
      else
      {
        // If selected sample types are specified, validate and construct a vector:
        std::for_each(types.cbegin(), types.cend(),
          [&available_types, &report_sample_types, this](const auto& type_prop)
        {
          auto it = std::find_if(available_types.cbegin(), available_types.cend(),
            [&type_prop](const auto& t) { return type_prop == Utilities::str2upper(t.second); });
          if (it != std::cend(available_types))
          {
            LOG_DEBUG << "Detected report-sample-types option: " << it->second;
            report_sample_types.insert(it->first);
          }
        });
      }
    }

    void ExportReport::_extract_report_metadata(
      const ApplicationSettings& application_settings,
      std::vector<FeatureMetadata>& report_metadata)
    {
      auto& available_metadata = SmartPeak::metadataToString;
      auto& meta = application_settings.report_metadata;
      if (ApplicationSettings::contains_option(
        meta, "ALL", "Detected report-metadata option: "))
      {
        // If no sample-types options are selected, use all features:
        std::transform(available_metadata.cbegin(), available_metadata.cend(),
          std::back_inserter(report_metadata), [](const auto& m) { return m.first; });
      }
      else
      {
        // If selected sample types are specified, validate and construct a vector:
        std::for_each(meta.cbegin(), meta.cend(),
          [&available_metadata, &report_metadata, this](const auto& meta_prop)
        {
          auto it = std::find_if(available_metadata.cbegin(), available_metadata.cend(),
            [&meta_prop](const auto& t) { return meta_prop == Utilities::str2upper(t.second); });
          if (it != std::cend(available_metadata))
          {
            LOG_DEBUG << "Detected report-metadata option: " << it->second;
            report_metadata.push_back(it->first);
          }
        });
      }
    }

  } /* namespace cli */
} /* namespace SmartPeak */
