#include "Taskbar.h"
#include "imgui.h"

void Taskbar::render(OSState& os) {
    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y + vp->Size.y - HEIGHT));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, HEIGHT));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.07f, 0.07f, 0.10f, 0.95f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
    ImGui::Begin("##taskbar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);

    // spec: at least 3 icon buttons. 2 = unique apps, 1 = task manager.
    if (iconButton("files",   "FILES", os.showFiles))    os.showFiles   = !os.showFiles;
    ImGui::SameLine();
    if (iconButton("taskmgr", "TASK",  os.showTaskMgr))  os.showTaskMgr = !os.showTaskMgr;
    ImGui::SameLine();
    if (iconButton("sysinfo", "INFO",  os.showSysInfo))  os.showSysInfo = !os.showSysInfo;

    // TODO [M3]: put a thin separator line between the buttons like the screenshot
    // TODO [M3]: add more buttons if you want (INIT / START / STOP / SRCH in the sample).
    //            not required, only the 3 above are

    drawTray();
    drawPwrButton(os);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

bool Taskbar::iconButton(const char* id, const char* label, bool active) {
    // TODO [M3]: make this look like an actual icon button.
    //   - draw a rounded rect / circle with the DrawList instead of a plain button
    //   - different color on hover (ImGui::IsItemHovered)
    //   - little underline or dot under it when 'active' is true (shows running app)
    //   - if you have icon textures use ImGui::ImageButton
    //   for now its just a normal button so everything is clickable
    ImGui::PushID(id);
    if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.7f, 1.f));
    bool clicked = ImGui::Button(label, ImVec2(60, 38));
    if (active) ImGui::PopStyleColor();
    ImGui::PopID();
    return clicked;
}

void Taskbar::drawTray() {
    // TODO [M3]: "VOL" and "NET" labels at the right side, left of PWR.
    //            can be just text, maybe clicking VOL shows a small slider popup
    //            (ImGui::BeginPopup + SliderInt) -- optional
}

void Taskbar::drawPwrButton(OSState& os) {
    // REQUIRED: this is the only legit way to close the app.
    // TODO [M2/M3]:
    //   - place the button at the far right of the taskbar (red text "PWR")
    //   - on click -> open a confirm popup ("Shut down CSOPESY OS?" Yes / No)
    //   - on Yes -> os.shutdownRequested = true;
    //   main.cpp already handles the rest (shutdown screen then closes the window)
    (void)os;
}
