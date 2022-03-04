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

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/WorkflowStepWidget.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct WorfklowStepNodeGraphContainer;

  struct WorfklowStepNode
  {
    ImVec2 getSize();
    ImVec2 getScreenPosition();
    int getWidth();
    int getHeight();
    virtual void draw(bool enable);
    bool isMouseIn();
    bool isCloseButtonMouseIn();

    ImVec2 pos_;
    int width_;
    ImVec2 drag_delta_;
    ApplicationHandler::Command command_;
    bool is_dragging_ = false;
    bool is_mouse_down_ = false;
    std::shared_ptr<WorfklowStepNodeGraphContainer> container_;

  protected:
    std::tuple<int, int, int, int> getCloseButtonPosition();
  };

  struct WorfklowStepNodePlaceHolder : public WorfklowStepNode
  {
    virtual void draw(bool enable);
  };

  struct WorfklowStepNodeGraphContainer
  {
    std::vector<WorfklowStepNode*> to_display_;
    ImVec2 pos_;
    void draw(bool enable);
    ImVec2 getSize();
    void layout();
    ApplicationHandler::Command::CommandType type_ = ApplicationHandler::Command::CommandType::RawDataMethod;
  };

  struct WorfklowStepNodeGraph
  {
    WorfklowStepNodeGraph(ApplicationHandler& application_handler, WorkflowManager& workflow_manager)
      : application_handler_(application_handler),
       workflow_manager_(workflow_manager),
       buildCommandsFromNames_(application_handler)
    {
    };
    std::vector<WorfklowStepNode> nodes;
    void draw();
  
  protected:
    virtual void updatecommands();
    void createContainers();
    void layout();

  protected:
    std::vector<WorfklowStepNode*> to_display_;
    std::vector<std::shared_ptr<WorfklowStepNodeGraphContainer>> containers_;
    WorfklowStepNode* dragging_node_ = nullptr;
    int dragging_node_index_ = 0;
    int place_holder_node_index_ = 0;
    WorfklowStepNodePlaceHolder place_holder_;
    WorkflowManager& workflow_manager_;
    ApplicationHandler& application_handler_;
    BuildCommandsFromNames buildCommandsFromNames_;
    bool error_building_commands_ = false;
    bool is_graph_hovered_ = false;
  };

  class WorkflowWidget : public Widget
  {
  public:

    WorkflowWidget(const std::string title, ApplicationHandler& application_handler, WorkflowManager& workflow_manager)
      : Widget(title),
      application_handler_(application_handler),
      workflow_step_widget_(application_handler),
      workflow_manager_(workflow_manager),
      workflow_node_graph_(application_handler, workflow_manager)
    {
    };

    void draw() override;

  protected:
    ApplicationHandler& application_handler_;
    WorkflowStepWidget workflow_step_widget_;
    WorkflowManager& workflow_manager_;
    WorfklowStepNodeGraph workflow_node_graph_;
  };
}
