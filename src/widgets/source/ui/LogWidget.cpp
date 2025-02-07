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

#include <SmartPeak/ui/LogWidget.h>
#include <imgui.h>
#include <plog/Log.h>

namespace SmartPeak
{

  void LogWidget::draw()
  {
    showQuickHelpToolTip("Log");
    
    const char* items[] = { "NONE", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "VERB" }; // reflects the strings in plog's Severity.h
    static int selected_severity = 4;
    static plog::Severity severity = plog::Severity::info;

    if (ImGui::Combo("Level", &selected_severity, items, IM_ARRAYSIZE(items)))
    {
      severity = plog::severityFromString(items[selected_severity]);
    }

    static bool wrap = true;
    ImGui::SameLine();
    ImGui::Checkbox("wrap", &wrap);

    static bool auto_scroll = true;
    ImGui::SameLine();
    ImGui::Checkbox("auto scroll", &auto_scroll);

    ImGui::Separator();
    ImGui::BeginChild("Log child");
    displayed_log_line_counter_ = 0;
    one_log_line_is_hovered_ = false;
    const auto record_list = appender_.getAppenderRecordList(severity);
    int message_list_start = (record_list.size() > 500) ? record_list.size() - 500 : 0;
    for (int i = message_list_start; i < record_list.size(); ++i)
    {
      std::string str(record_list.at(i).second.data(), record_list.at(i).second.data() + record_list.at(i).second.size());
      
      if (record_list.at(i).first == plog::Severity::fatal) {
        displayLogLine(str.c_str(), ImVec4(1.0f, 0.0f, 0.0f, 1.0f), wrap);
      } else if (record_list.at(i).first == plog::Severity::error) {
        displayLogLine(str.c_str(), ImVec4(8.0f, 0.15f, 0.15f, 1.0f), wrap);
      } else if (record_list.at(i).first == plog::Severity::warning) {
        displayLogLine(str.c_str(), ImVec4(8.0f, 0.5f, 0.5f, 1.0f), wrap);
      } else if (record_list.at(i).first == plog::Severity::info) {
        displayLogLine(str.c_str(), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), wrap);
      } else if (record_list.at(i).first == plog::Severity::debug) {
        displayLogLine(str.c_str(), ImVec4(1.0f, 0.6f, 0.0f, 1.0f), wrap);
      } else if (record_list.at(i).first == plog::Severity::verbose) {
        displayLogLine(str.c_str(), ImVec4(0.0f, 0.0f, 0.0f, 1.0f), wrap);
      }
    }
    if (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
      ImGui::SetScrollHereY(1.0f);
    }
    if (!one_log_line_is_hovered_)
    {
      hovered_log_line_ = -1;
    }
    ImGui::EndChild();
  }

  void LogWidget::displayLogLine(const char* str, const ImVec4& color, bool wrap)
  {
    if (hovered_log_line_ == displayed_log_line_counter_)
    {
      ImVec2 text_size = ImGui::CalcTextSize(str, 0, false, ImGui::GetWindowWidth());
      const ImVec2 posR = ImGui::GetCursorScreenPos();
      ImDrawList* draw_list = ImGui::GetWindowDrawList();
      draw_list->AddRectFilled(
        ImVec2(posR.x, posR.y),
        ImVec2(posR.x + ImGui::GetWindowWidth(), posR.y + text_size.y),
        IM_COL32(200, 200, 200, 30));
    }
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    if (wrap)
    {
      ImGui::TextWrapped("%s", str);
    }
    else
    {
      ImGui::Text("%s", str);
    }
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered())
    {
      hovered_log_line_ = displayed_log_line_counter_;
      one_log_line_is_hovered_ = true;
    }
    displayed_log_line_counter_++;
  }
}
