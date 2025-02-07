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
// $Maintainer: Bertrand Boudaud, Ahmed Khalil, Douglas McCloskey $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/Plotter.h>
#include <SmartPeak/ui/FilePicker.h>

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>

namespace SmartPeak
{

  /**
    @brief Class for plotting heatmaps
  */
  class Heatmap2DWidget : 
    public GenericGraphicWidget,
    public ISequenceObserver
  {
  public:
    Heatmap2DWidget(SessionHandler& session_handler,
                    ApplicationHandler& application_handler,
                    const std::string& id,
                    const std::string& title,
                    SequenceObservable& sequence_observable)
      : GenericGraphicWidget(title),
        session_handler_(session_handler),
        sequence_handler_(application_handler.sequenceHandler_),
        application_handler_(application_handler),
        plot_title_(id) 
    {
      sequence_observable.addSequenceObserver(this);
    };
    void draw() override;

  public:
    /**
     ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

  private:
    bool compareInput(const Eigen::Tensor<std::string, 1>& l, const Eigen::Tensor<std::string, 1>& r) const;
  protected:
    SessionHandler& session_handler_;
    SequenceHandler& sequence_handler_;
    ApplicationHandler& application_handler_;
    SessionHandler::HeatMapData heatmap_data_;
    std::string plot_title_; // used as the ID of the plot as well so this should be unique across the different Widgets
    std::string selected_feature_;
    bool invalid_data_;
    bool data_mismatch_;
    bool refresh_needed_ = false;
  };

}
