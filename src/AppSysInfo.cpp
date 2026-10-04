#include "AppSysInfo.h"
#include "imgui.h"

void AppSysInfo::render(OSState& os) {
    if (!os.showSysInfo) return;

    ImGui::SetNextWindowSize(ImVec2(420, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(260, 160), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("System Information", &os.showSysInfo)) {
        ImGui::Text("CSOPESY OS v1.0");
        ImGui::Separator();

        // TODO [M4]: make the real screen. ideas:
        //   - key/value list: CPU, RAM, BIOS version, disks (copy from the bios screen)
        //   - a tab bar (BeginTabBar): General / Display / About
        //   - fake settings with checkboxes and sliders (brightness, volume, theme)
        //   - ImGui::PlotLines for a fake cpu graph
        ImGui::TextDisabled("(placeholder)");
    }
    ImGui::End();
}
