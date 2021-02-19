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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/ui/Widget.h>
#include <algorithm>
#include <array>
#include <string>
#include <vector>
#include <atomic>
// #include <boost/filesystem.hpp>

// namespace fs = boost::filesystem;

namespace SmartPeak
{
  struct ImDirectoryEntry;
  class FilePicker final : public Widget
  {
    std::array<std::vector<std::string>, 4> pathname_content_;
    std::string current_pathname_ = ".";
    std::string picked_pathname_;
    FilePickerProcessor* processor_ = nullptr;
    std::string processor_name_ = "";
    bool loading_is_done_ = true;
    bool file_was_loaded_ = true;
    bool error_loading_file_ = false;
    std::atomic_bool files_scanned {false};

    void run_and_join(
      FilePickerProcessor* processor,
      const std::string& pathname,
      bool& loading_is_done,
      bool& file_was_loaded
    );
    
    ///!  rescan pathname_content_ into content_items when needed
    void update_contents(ImVector<ImDirectoryEntry>& content_items);

  public:
    FilePicker()
    {
    }

    void draw() override;
    std::string getPickedPathname() const;
    void setProcessor(FilePickerProcessor& processor);
    const std::string getProcessorName() const { return processor_name_; };
    void runProcessor();
    void clearProcessor();
    bool fileLoadingIsDone() { return loading_is_done_; };
    bool fileWasLoaded() { return file_was_loaded_; };
    bool errorLoadingFile() { return error_loading_file_; };
  };
}
