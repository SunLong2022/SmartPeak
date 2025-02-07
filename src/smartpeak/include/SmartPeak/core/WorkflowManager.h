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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>

namespace SmartPeak {
  //TODO: implement a detailed workflow status
  enum class WorkFlowStatus
  {
    IDLE,
    RUNNING,
    FINISHED,
    SIZE_OF_WORKFLOWSTATUS
  };

  class WorkflowManager {
  public:
    /**
      Copies the passed application_handler and sets up the async run of the workflow. Only one
      workflow is managed at a time

      @param[in,out] The application_handler that gets copied and then updated at the end of the workflow run
      @param[in] injection_names Injection names to use for Sequence Processing
      @param[in] sequence_segment_names Sequence Segment Names to use for Sequence Segment Processing
      @param[in] sample_group_names Sample Group Names to use for Sample Group Processing
      @param[in] commands Workflow steps
      @param[in] number_of_threads number of threads to use to execute the workflow
      @param[in] sequence_processor_observer an observer for sequence, used to report progress
      @param[in] sequence_segment_processor_observer an observer for sequence segment, used to report progress
      @param[in] sample_group_processor_observer, used to report progress
      @param[in] blocking If true the operation runs synchronously, otherwise runs asynchronously and returns immediately
    */
    void addWorkflow(ApplicationHandler& source_state, 
      const std::set<std::string>& injection_names, 
      const std::set<std::string>& sequence_segment_names, 
      const std::set<std::string>& sample_group_names, 
      const std::vector<ApplicationHandler::Command>& commands, 
      const int number_of_threads,
      IApplicationProcessorObserver* application_processor_observer = nullptr,
      ISequenceProcessorObserver* sequence_processor_observer = nullptr,
      ISequenceSegmentProcessorObserver * sequence_segment_processor_observer = nullptr,
      ISampleGroupProcessorObserver * sample_group_processor_observer = nullptr,
      bool blocking=false);

    /**
      If this returns false, new workflows can't run and the following menu items
      are disabled in the UI:
      - File -> Load session from sequence
      - Edit -> Workflow
      - Action -> Run command
      - Action -> Run workflow
    */
    bool isWorkflowDone() const;

    /**
      Update the application handler with the resulting application handler
      must be called from the main thread.
    */
    void updateApplicationHandler(ApplicationHandler& source_app_handler);

    /**
      @brief Returns the list of requirements to run the workflow.
    */
    std::set<std::string> getRequirements(const std::vector<ApplicationHandler::Command>& commands) const;

    /**
      @brief return true if the Filenames contains all the requirements to run the commands
    */
    bool isMissingRequirements(const Filenames& filenames, const std::set<std::string>& requirements) const;

  private:
    /**
      Spawns a thread that runs the workflow, and waits for it to finish. The
      modified application_handler is copied back to the source, keeping the application_handler of the app
      up-to-date

      @param[in,out] application_handler Points to the class' application_handler member
      @param[in,out] done Points to the class' done member
      @param[in,out] run_time time taken to run the workflow
      @param[out] source_app_handler The modified application_handler is copied back here
      @param[in] injection_names Injection names to use for Sequence Processing
      @param[in] sequence_segment_names Sequence Segment Names to use for Sequence Segment Processing
      @param[in] sample_group_names Sample Group Names to use for Sample Group Processing
      @param[in] commands Workflow steps
      @param[in] number_of_threads number of threads to use to execute the workflow
      @param[in] sequence_processor_observer an observer for sequence, used to report progress
      @param[in] sequence_segment_processor_observer an observer for sequence segment, used to report progress
      @param[in] sample_group_processor_observer, used to report progress
    */
    static void run_and_join(ApplicationHandler& application_handler, 
      bool& done,
      const std::set<std::string>& injection_names, 
      const std::set<std::string>& sequence_segment_names,
      const std::set<std::string>& sample_group_names,
      const std::vector<ApplicationHandler::Command>& commands,
      const int number_of_threads,
      IApplicationProcessorObserver* application_processor_observer = nullptr,
      ISequenceProcessorObserver* sequence_processor_observer = nullptr,
      ISequenceSegmentProcessorObserver * sequence_segment_processor_observer = nullptr,
      ISampleGroupProcessorObserver* sample_group_processor_observer = nullptr);

    ApplicationHandler application_handler_; ///< The workflow is run on this copy
    bool done_ = true;
  };
}
