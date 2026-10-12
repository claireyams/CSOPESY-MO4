#include "Taskbar.h"
#include "imgui.h"
#include <cstring>

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
    if (iconButton("files", "FILE EXPLORER", os.showFiles)) os.showFiles = !os.showFiles;

    // thin vertical line after the first button
    ImGui::SameLine();
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + 6.f, p.y + 4.f),
                                            ImVec2(p.x + 6.f, p.y + 36.f),
                                            IM_COL32(255, 255, 255, 50), 1.f);
        ImGui::Dummy(ImVec2(12.f, 40.f));
    }

    ImGui::SameLine();
    if (iconButton("sysinfo", "SYSTEM INFORMATION", os.showSysInfo)) os.showSysInfo = !os.showSysInfo;
    ImGui::SameLine();
    if (iconButton("taskmgr", "TASK MANAGER", os.showTaskMgr)) os.showTaskMgr = !os.showTaskMgr;

    drawTray();
    drawPwrButton(os);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

bool Taskbar::iconButton(const char* id, const char* label, bool active) {
    const ImVec2 size(60.f, 40.f);

    ImGui::PushID(id);
    ImVec2 pos = ImGui::GetCursorScreenPos();
    bool clicked = ImGui::InvisibleButton("##btn", size);
    bool hovered = ImGui::IsItemHovered();
    bool pressed = ImGui::IsItemActive();
    ImGui::PopID();

    ImU32 bg = IM_COL32(38, 40, 56, 255);
    if (active)  bg = IM_COL32(45, 85, 150, 255);
    if (hovered) bg = active ? IM_COL32(60, 105, 175, 255) : IM_COL32(58, 62, 84, 255);
    if (pressed) bg = IM_COL32(30, 60, 110, 255);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);
    dl->AddRectFilled(pos, end, bg, 10.f);

    // icon drawn with shapes, centered in the button
    ImU32 ic = IM_COL32(235, 235, 245, 255);
    ImVec2 c = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f - (active ? 3.f : 0.f));
    if (strcmp(id, "files") == 0) {
        // folder
        dl->AddRectFilled(ImVec2(c.x - 10, c.y - 8), ImVec2(c.x - 1, c.y - 4), ic, 2.f);
        dl->AddRectFilled(ImVec2(c.x - 10, c.y - 5), ImVec2(c.x + 10, c.y + 8), ic, 2.f);
    } else if (strcmp(id, "taskmgr") == 0) {
        // bar chart
        dl->AddRectFilled(ImVec2(c.x - 10, c.y - 2), ImVec2(c.x - 5, c.y + 8), ic);
        dl->AddRectFilled(ImVec2(c.x - 3,  c.y - 9), ImVec2(c.x + 2, c.y + 8), ic);
        dl->AddRectFilled(ImVec2(c.x + 4,  c.y + 1), ImVec2(c.x + 9, c.y + 8), ic);
    } else if (strcmp(id, "sysinfo") == 0) {
        // circled "i"
        dl->AddCircle(c, 10.f, ic, 24, 2.f);
        dl->AddCircleFilled(ImVec2(c.x, c.y - 4.5f), 1.6f, ic);
        dl->AddLine(ImVec2(c.x, c.y - 1.f), ImVec2(c.x, c.y + 5.f), ic, 2.f);
    }

    // name shows up when you hover, like a real taskbar
    if (hovered) ImGui::SetTooltip("%s", label);

    // small bar under the label means the app is open
    if (active) {
        float cx = pos.x + size.x * 0.5f;
        dl->AddRectFilled(ImVec2(cx - 8.f, end.y - 7.f), ImVec2(cx + 8.f, end.y - 4.f),
                          IM_COL32(120, 190, 255, 255), 2.f);
    }

    return clicked;
}

void Taskbar::drawTray() {
    static int  volume = 60;
    static bool muted  = false;
    static bool wifiOn = true;

    float w = ImGui::GetWindowWidth();

    // flat buttons, only light up on hover
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 0.10f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1.f, 1.f, 1.f, 0.20f));

    // VOL
    ImGui::SameLine(w - 200.f);
    ImGui::PushStyleColor(ImGuiCol_Text, muted ? ImVec4(0.6f, 0.6f, 0.6f, 1.f)
                                               : ImVec4(1.f, 1.f, 1.f, 1.f));
    bool volClicked = ImGui::Button("VOL", ImVec2(50, 38));
    ImGui::PopStyleColor();
    ImVec2 volTop = ImVec2((ImGui::GetItemRectMin().x + ImGui::GetItemRectMax().x) * 0.5f,
                           ImGui::GetItemRectMin().y);
    if (volClicked) ImGui::OpenPopup("##volpopup");

    // centered above the button so it never runs off the right edge
    ImGui::SetNextWindowPos(ImVec2(volTop.x, volTop.y - 6.f), ImGuiCond_Always, ImVec2(0.5f, 1.f));
    if (ImGui::BeginPopup("##volpopup")) {
        ImGui::Text("Volume");
        ImGui::SetNextItemWidth(160.f);
        ImGui::SliderInt("##vol", &volume, 0, 100);
        ImGui::Checkbox("Mute", &muted);
        ImGui::EndPopup();
    }

    // NET
    ImGui::SameLine(w - 140.f);
    ImGui::PushStyleColor(ImGuiCol_Text, wifiOn ? ImVec4(0.4f, 1.f, 0.5f, 1.f)
                                                : ImVec4(0.6f, 0.6f, 0.6f, 1.f));
    bool netClicked = ImGui::Button("NET", ImVec2(50, 38));
    ImGui::PopStyleColor();
    ImVec2 netTop = ImVec2((ImGui::GetItemRectMin().x + ImGui::GetItemRectMax().x) * 0.5f,
                           ImGui::GetItemRectMin().y);
    if (netClicked) ImGui::OpenPopup("##netpopup");

    ImGui::SetNextWindowPos(ImVec2(netTop.x, netTop.y - 6.f), ImGuiCond_Always, ImVec2(0.5f, 1.f));
    if (ImGui::BeginPopup("##netpopup")) {
        ImGui::Text("Network");
        ImGui::Separator();
        ImGui::Checkbox("Wi-Fi", &wifiOn);
        if (wifiOn) ImGui::TextDisabled("Connected: CSOPESY-LAN");
        else        ImGui::TextDisabled("Not connected");
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(3);
}

void Taskbar::drawPwrButton(OSState& os) {
    // the only way to close the app. main.cpp does the rest once
    // shutdownRequested is set.
    ImGui::SameLine(ImGui::GetWindowWidth() - 80.f);

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 0.25f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.9f, 0.2f, 0.2f, 0.45f));
    ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(1.f, 0.3f, 0.3f, 1.f));
    if (ImGui::Button("PWR", ImVec2(60, 38))) {
        ImGui::OpenPopup("Shut down?");
    }
    ImGui::PopStyleColor(4);

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Shut down?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Shut down CSOPESY OS?");
        ImGui::Separator();

        if (ImGui::Button("Yes", ImVec2(100, 0))) {
            os.shutdownRequested = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(100, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}