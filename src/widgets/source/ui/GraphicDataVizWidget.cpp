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

#include <SmartPeak/ui/GraphicDataVizWidget.h>
#include <SmartPeak/ui/FilePicker.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace SmartPeak
{

  void GraphicDataVizWidget::draw()
  {
    updateData();
    drawGraphHeader();
    drawGraph();
  }

  void GraphicDataVizWidget::drawGraphHeader()
  {
    float controls_pos_start_y = ImGui::GetCursorPosY();
    ImGui::Checkbox("Compact View", &compact_view_);
    ImGui::SameLine();
    ImGui::Checkbox("Legend", &show_legend_);
    ImGui::SameLine();
    if (ImGui::Button("Fit Zoom"))
    {
      update_plot_range_ = true;
    }

    static FilePicker file_picker_;
    
    if (ImGui::Button("Choose folder"))
    {
      file_picker_.open("Choose folder to export plot", nullptr, SmartPeak::FilePicker::Mode::EDirectory, application_handler_);
    }
    file_picker_.draw();
    ImGui::SameLine();
    static int selected_format = 0;
    static const char* formats[] = { "Save As PNG", "Save As PDF", "Save As HTML", "Save As SVG"};
    ImGui::Combo(" ", &selected_format, formats, IM_ARRAYSIZE(formats));
    ImGui::SameLine();
    if (ImGui::Button("Save Plot"))
    {
      auto exported_plot = std::make_unique<PlotExporter>(
        file_picker_.getPickedPathname().empty() ? application_handler_.main_dir_.string() : file_picker_.getPickedPathname(),
        graph_viz_data_, selected_format, PlotExporter::PlotType::CURVE);
      if (!exported_plot->plot()) show_installation_guide_ = true;
    }
    ImGui::Spacing();
    
    if (show_installation_guide_) showInstallationInstructions();
  }

  std::tuple<float, float, float, float> GraphicDataVizWidget::plotLimits() const
  {
    // compute plot limit by adding padding
    float border_padding_x = (graph_viz_data_.x_max_ - graph_viz_data_.x_min_) * 0.01f;
    float border_padding_y = (graph_viz_data_.y_max_ - graph_viz_data_.y_min_) * 0.01f;
    auto plot_min_x = graph_viz_data_.x_min_ - border_padding_x;
    auto plot_max_x = graph_viz_data_.x_max_ + border_padding_x;
    auto plot_min_y = graph_viz_data_.y_min_ - border_padding_y;
    auto plot_max_y = graph_viz_data_.y_max_ * 1.1f;
    return std::make_tuple(plot_min_x, plot_max_x, plot_min_y, plot_max_y);
  }

  // update ranges after having refreshed the graph
  void GraphicDataVizWidget::updateRanges()
  {
    search_highest_value_ = true;
    update_plot_range_ = true;
  }

  void GraphicDataVizWidget::drawGraph()
  {
    // Main graphic
    if (graph_viz_data_.points_overflow_)
    {
      ImGui::Text("Unable to draw: too much points. Please reduce scope or unselect data.");
    }
    else if (graph_viz_data_.x_data_area_.empty() && graph_viz_data_.y_data_area_.empty())
    {
      ImGui::Text("No data to display select data to render or adjust ranges.");
    }
    else
    {
      auto window_size = ImGui::GetWindowSize();
      ImGuiCond cond;
      cond = ImGuiCond_Once;
      if (update_plot_range_)
      {
        cond = ImGuiCond_Always;
        update_plot_range_ = false;
      }
      if (restore_plot_limits_)
      {
        restore_plot_limits_ = false;
        ImPlot::SetNextPlotLimits(plot_limits_.X.Min, plot_limits_.X.Max, plot_limits_.Y.Min, plot_limits_.Y.Max, ImGuiCond_Always);
      }
      else
      {
        auto [plot_min_x, plot_max_x, plot_min_y, plot_max_y] = plotLimits();
        ImPlot::SetNextPlotLimits(plot_min_x, plot_max_x, plot_min_y, plot_max_y, cond);
      }
      ImPlotFlags plotFlags = show_legend_ ? ImPlotFlags_Default | ImPlotFlags_Legend : ImPlotFlags_Default & ~ImPlotFlags_Legend;
      plotFlags |= ImPlotFlags_Crosshairs;
      float graphic_height = window_size.y;
      if (ImPlot::BeginPlot(plot_title_.c_str(), graph_viz_data_.x_axis_title_.c_str(), graph_viz_data_.y_axis_title_.c_str(), ImVec2(window_size.x - 25, graphic_height - 85), plotFlags)) {
        int i = 0;
        for (const auto& serie_name_scatter : graph_viz_data_.series_names_area_)
        {
          if (graph_viz_data_.x_data_area_.size())
          {
            assert(graph_viz_data_.x_data_area_.at(i).size() == graph_viz_data_.y_data_area_.at(i).size());
            ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_None);
            if (!is_spectra_)
            {
              ImPlot::PlotLine(serie_name_scatter.c_str(),
                               graph_viz_data_.x_data_area_.at(i).data(),
                               graph_viz_data_.y_data_area_.at(i).data(),
                               graph_viz_data_.x_data_area_.at(i).size());
            }
            else
            {
              static const float bar_width_in_pixel = 3.0f;
              auto bar_width = ImPlot::PixelsToPlot({ bar_width_in_pixel ,0.0f }).x - ImPlot::PixelsToPlot({ 0.0f,0.0f }).x;
              ImPlot::PlotBars(serie_name_scatter.c_str(),
                               graph_viz_data_.x_data_area_.at(i).data(), 
                               graph_viz_data_.y_data_area_.at(i).data(), 
                               graph_viz_data_.x_data_area_.at(i).size(),
                               bar_width);
            }
          }
          plot_limits_ = ImPlot::GetPlotLimits();
          plotHighestValue(i);
          
          ImPlotMarker plot_marker = ImPlotMarker_Circle;
          int feature_index = 0;
          for (int j = 0; j < graph_viz_data_.x_data_scatter_.size(); ++j) {
            // Corresponding serie names are supposed to start with same name as the scatter name
            if (graph_viz_data_.series_names_scatter_.at(j).rfind(serie_name_scatter) == 0)
            {
              assert(graph_viz_data_.x_data_scatter_.at(j).size() == graph_viz_data_.y_data_scatter_.at(j).size());
              ImPlot::PushStyleVar(ImPlotStyleVar_Marker, plot_marker);
              std::string legend_text = serie_name_scatter;
              if (!compact_view_)
              {
                legend_text = graph_viz_data_.series_names_scatter_.at(j) + "::" + std::to_string(feature_index);
              }
              ImPlot::PlotScatter(legend_text.c_str(), 
                                  graph_viz_data_.x_data_scatter_.at(j).data(), 
                                  graph_viz_data_.y_data_scatter_.at(j).data(),
                                  graph_viz_data_.x_data_scatter_.at(j).size());
              plot_marker <<= 1;
              if (plot_marker > ImPlotMarker_Asterisk) plot_marker = ImPlotMarker_Circle;
              ++feature_index;
            }
          }
          ++i;
        }
        drawMarkers();
        ImPlot::EndPlot();
      }
    }
  }

  void GraphicDataVizWidget::drawMarkers()
  {
    // draw placed marker
    if (use_markers_)
    {
      const auto marker_position = getMarkerPosition();
      auto [plot_min_x, plot_max_x, plot_min_y, plot_max_y] = plotLimits();
      if (marker_position)
      {
        float marker_line_x[] = { *marker_position, *marker_position };
        float marker_line_y[] = { plot_min_y, plot_max_y };
        ImPlot::PushStyleColor(0, ImGui::GetStyle().Colors[ImGuiCol_Text]);
        ImPlot::PlotLine("", marker_line_x, marker_line_y, 2);
        ImPlot::PopStyleColor();
      }
      // mouse hover marker
      if (ImPlot::IsPlotHovered())
      {
        auto plot_point = ImPlot::GetPlotMousePos();
        auto nearest_point = getNearestPoint(plot_point.x);
        if (nearest_point)
        {
          float marker_line_x[] = { *nearest_point, *nearest_point };
          float marker_line_y[] = { plot_min_y, plot_max_y };
          ImPlot::PushStyleColor(0, ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]);
          ImPlot::PlotLine("", marker_line_x, marker_line_y, 2);
          ImPlot::PopStyleColor();
          if (ImGui::IsMouseClicked(0))
          {
            setMarkerPosition(nearest_point);
          }
        }
      }
    }
  }

  std::optional<float> GraphicDataVizWidget::getMarkerPosition() const
  {
    return marker_position_;
  }

  void GraphicDataVizWidget::setMarkerPosition(const std::optional<float>& marker_position)
  {
    marker_position_ = marker_position;
  }

  std::set<std::string> GraphicDataVizWidget::getSelectedSampleNames() const
  {
    std::set<std::string> sample_names;
    Eigen::Tensor<std::string, 1> selected_sample_names = session_handler_.getSelectSampleNamesPlot();
    for (int i = 0; i < selected_sample_names.size(); ++i) {
      if (!selected_sample_names(i).empty())
        sample_names.insert(selected_sample_names(i));
    }
    return sample_names;
  }

  std::set<std::string> GraphicDataVizWidget::getSelectedTransitions() const
  {
    std::set<std::string> transitions_names;
    Eigen::Tensor<std::string, 1> selected_transitions = session_handler_.getSelectTransitionsPlot();
    for (int i = 0; i < selected_transitions.size(); ++i) {
      if (!selected_transitions(i).empty())
        transitions_names.insert(selected_transitions(i));
    }
    return transitions_names;
  }

  std::set<std::string> GraphicDataVizWidget::getSelectedTransitionGroups() const
  {
    std::set<std::string> component_group_names;
    Eigen::Tensor<std::string, 1> selected_transition_groups = session_handler_.getSelectTransitionGroupsPlot();
    for (int i = 0; i < selected_transition_groups.size(); ++i) {
      if (!selected_transition_groups(i).empty())
        component_group_names.insert(selected_transition_groups(i));
    }
    return component_group_names;
  }

  std::optional<float> GraphicDataVizWidget::getNearestPoint(float in_x) const
  {
    std::optional<float> result = {};
    float current_distance = std::numeric_limits<float>::max();
    size_t nearest_serie_index = -1;
    for (const auto& scatter_x : graph_viz_data_.x_data_area_)
    {
      for (const auto& pos_x : scatter_x)
      {
        auto distance_test = std::abs(pos_x - in_x);
        if (distance_test < current_distance)
        {
          current_distance = distance_test;
          result = pos_x;
        }
      }
    }
    return result;
  }

  void GraphicDataVizWidget::showInstallationInstructions()
  {
    ImGui::OpenPopup("gnuplot program could not be found! Instructions:");
    if (ImGui::BeginPopupModal("gnuplot program could not be found! Instructions:", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      
#if defined(__APPLE__)
      ImGui::Text("In your terminal install brew package manager if you haven't : ");
      ImGui::Text("   /bin/bash -c \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\" ");
      ImGui::Text("Then install gnuplot : ");
      ImGui::Text("   brew install gnuplot");
#elif _WIN32
      ImGui::Separator();
      ImGui::Text("Download and install from http://ftp.cstug.cz/pub/CTAN/graphics/gnuplot/5.2.6/gp526-win64-mingw_2.exe ");
#endif
      ImGui::Separator();
      if (ImGui::Button("Close"))
      {
        show_installation_guide_ = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  void GraphicDataVizWidget::plotHighestValue(int idx)
  {
    if (search_highest_value_ || refresh_needed_)
    {
      highest_values_x_.clear(); highest_values_y_.clear();
      if (graph_viz_data_.x_data_area_.size() && graph_viz_data_.y_data_area_.size())
      {
        for (int i=0; i < graph_viz_data_.x_data_area_.size(); ++i)
        {
          if (graph_viz_data_.x_data_area_.at(i).size() && graph_viz_data_.y_data_area_.at(i).size())
          {
            auto highest_value_it = std::max_element(
              graph_viz_data_.y_data_area_.at(i).begin(),
              graph_viz_data_.y_data_area_.at(i).end());
           
            highest_values_x_.push_back(
              static_cast<float>(graph_viz_data_.x_data_area_.at(i).at(
                std::distance(graph_viz_data_.y_data_area_.at(i).begin(), highest_value_it))));
            highest_values_y_.push_back(*highest_value_it);
          }
        }
      }
      search_highest_value_ = false;
    }
    
    if (highest_values_x_.size() == highest_values_y_.size())
    {
      ImPlot::PushStyleColor(0, ImVec4(ImColor(255,255,255)));
      ImPlot::PlotScatter("", // do not appear in legend
        highest_values_x_.data(),
        highest_values_y_.data(),
        highest_values_x_.size());
      
      for (int idx = 0; idx < highest_values_x_.size(); ++idx)
      {
        std::stringstream highest_value_ss;
        highest_value_ss << std::fixed << std::setprecision(1) << highest_values_y_[idx];
        ImPlot::PlotText(
          highest_value_ss.str().c_str(),
          highest_values_x_[idx] + 0.2f,
          highest_values_y_[idx] + 0.8f);
        }
      
      ImPlot::PopStyleColor();
    }
  }

  std::map<std::string, CastValue::Type> GraphicDataVizWidget::getPropertiesSchema() const
  {
    auto properties = Widget::getPropertiesSchema();
    // sliders ranges
    properties.emplace("plot_limits_.X.Max", CastValue::Type::FLOAT);
    properties.emplace("plot_limits_.X.Min", CastValue::Type::FLOAT);
    properties.emplace("plot_limits_.Y.Max", CastValue::Type::FLOAT);
    properties.emplace("plot_limits_.Y.Min", CastValue::Type::FLOAT);
    properties.emplace("compact_view_", CastValue::Type::BOOL);
    properties.emplace("show_legend_", CastValue::Type::BOOL);
    properties.emplace("marker_position_", CastValue::Type::FLOAT);
    return properties;
  }

  std::optional<CastValue> GraphicDataVizWidget::getProperty(const std::string& property, const size_t row) const
  {
    auto widget_field = Widget::getProperty(property, row);
    if (widget_field)
    {
      return widget_field;
    }
    if (property == "plot_limits_.X.Max")
    {
      return static_cast<float>(plot_limits_.X.Max);
    }
    if (property == "plot_limits_.X.Min")
    {
      return static_cast<float>(plot_limits_.X.Min);
    }
    if (property == "plot_limits_.Y.Max")
    {
      return static_cast<float>(plot_limits_.Y.Max);
    }
    if (property == "plot_limits_.Y.Min")
    {
      return static_cast<float>(plot_limits_.Y.Min);
    }
    if (property == "compact_view_")
    {
      return compact_view_;
    }
    if (property == "show_legend_")
    {
      return show_legend_;
    }
    if (property == "marker_position_")
    {
      return marker_position_ ? *marker_position_ : 0;
    }
    return std::nullopt;
  }

  void GraphicDataVizWidget::setProperty(const std::string& property, const CastValue& value, const size_t row)
  {
    Widget::setProperty(property, value, row);
    if (property == "compact_view_")
    {
      compact_view_ = value.b_;
    }
    if (property == "show_legend_")
    {
      show_legend_ = value.b_;
    }
    if (property == "plot_limits_.X.Max")
    {
      plot_limits_.X.Max = value.f_;
      restore_plot_limits_ = true;
    }
    if (property == "plot_limits_.X.Min")
    {
      plot_limits_.X.Min = value.f_;
      restore_plot_limits_ = true;
    }
    if (property == "plot_limits_.Y.Max")
    {
      plot_limits_.Y.Max = value.f_;
      restore_plot_limits_ = true;
    }
    if (property == "plot_limits_.Y.Min")
    {
      plot_limits_.Y.Min = value.f_;
      restore_plot_limits_ = true;
    }
    if (property == "marker_position_")
    {
      marker_position_ = value.f_;
    }
  }
}
