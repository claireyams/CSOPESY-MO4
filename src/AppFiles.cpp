#include "AppFiles.h"
#include "imgui.h"

void AppFiles::render(OSState& os) {
    if (!os.showFiles) return;

    ImGui::SetNextWindowSize(ImVec2(520, 340), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(200, 120), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("File Explorer", &os.showFiles)) {
        ImGui::Text("This PC > Local Disk (C:)");
        ImGui::Separator();

        // TODO [M4]: build the actual layout. ideas:
        //   - left panel (BeginChild) with a folder tree (TreeNode)
        //   - right panel with a fake list of files (Selectable) + size/date columns
        //   - status bar at the bottom ("5 items")
        //   all placeholder data is fine, nothing needs to touch the real disk
        ImGui::TextDisabled("(placeholder)");
    }
    ImGui::End();
}
