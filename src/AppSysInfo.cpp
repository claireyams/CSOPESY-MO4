#include "AppSysInfo.h"
#include "Taskbar.h"
#include "imgui.h"
#include <algorithm>
#include <cstdio>

namespace {
// shared label/value row; long values wrap when the window is narrow
void infoRow(const char* label, const char* value) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextDisabled("%s", label);
    ImGui::TableSetColumnIndex(1);
    ImGui::TextWrapped("%s", value);
}

// fixed label width keeps the values lined up across all sections
bool beginInfoTable(const char* id) {
    if (!ImGui::BeginTable(id, 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) return false;
    ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 130.f);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
    return true;
}

void sectionTitle(const char* title) {
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.47f, 0.75f, 1.f, 1.f), "%s", title);
    ImGui::Separator();
}

// keep the window above the taskbar when the desktop is resized
void keepWindowInDesktop() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    const ImVec2 pos = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetWindowSize();
    ImGui::SetWindowPos(ImVec2(
        std::max(vp->Pos.x + 16.f, std::min(pos.x, vp->Pos.x + vp->Size.x - size.x - 16.f)),
        std::max(vp->Pos.y + 16.f, std::min(pos.y, vp->Pos.y + vp->Size.y - Taskbar::HEIGHT - size.y - 16.f))));
}
}

void AppSysInfo::render(OSState& os) {
    if (!os.showSysInfo) return;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    // reserve 16 px at each edge and leave room for the taskbar
    const ImVec2 available(std::max(1.f, vp->Size.x - 32.f),
                           std::max(1.f, vp->Size.y - Taskbar::HEIGHT - 32.f));
    const ImVec2 size(std::min(520.f, available.x), std::min(440.f, available.y));
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x + vp->Size.x - size.x - 16.f,
                                  vp->Pos.y + 16.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(std::min(360.f, available.x),
                                               std::min(260.f, available.y)), available);
    const bool expanded = ImGui::Begin("System Information", &os.showSysInfo);
    keepWindowInDesktop();
    if (expanded) {
        ImGui::TextColored(ImVec4(0.47f, 0.75f, 1.f, 1.f), "CSOPESY OS");
        ImGui::SameLine();
        ImGui::TextDisabled("v1.0");
        ImGui::TextDisabled("Your system at a glance");
        ImGui::Spacing();

        // each tab gets its own scrolling region for smaller windows
        if (ImGui::BeginTabBar("systemTabs")) {
            if (ImGui::BeginTabItem("General")) {
                if (ImGui::BeginChild("generalContent", ImVec2(0, 0))) drawGeneral();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Display")) {
                if (ImGui::BeginChild("displayContent", ImVec2(0, 0))) drawDisplay();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("About")) {
                if (ImGui::BeginChild("aboutContent", ImVec2(0, 0))) drawAbout();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

// hardware and storage values match the simulated BIOS in BootScreen.cpp
void AppSysInfo::drawGeneral() {
    sectionTitle("Operating System");
    if (beginInfoTable("osDetails")) {
        infoRow("Name", "CSOPESY OS");
        infoRow("Version", "1.0");
        infoRow("Status", "System Online");
        ImGui::EndTable();
    }

    sectionTitle("Hardware");
    ImGui::TextWrapped("Simulated specifications matching the boot screen.");
    if (beginInfoTable("hardwareDetails")) {
        infoRow("Processor", "Pentium III");
        infoRow("Numeric Processor", "Built-in");
        infoRow("RAM", "64000K");
        infoRow("BIOS", "BCN SIT 1989-1994 Special CSOPESY");
        infoRow("BIOS Release", "12/01/94");
        ImGui::EndTable();
    }

    sectionTitle("Storage");
    if (beginInfoTable("storageDetails")) {
        infoRow("Primary Master", "420 MB WDCAC2420H");
        infoRow("Primary Slave", "None");
        infoRow("Secondary Master", "CD-ROM LTN-385A");
        infoRow("Secondary Slave", "None");
        ImGui::EndTable();
    }
}

// read ImGui's metrics each frame so the Display tab stays up to date
void AppSysInfo::drawDisplay() {
    const ImGuiIO& io = ImGui::GetIO();
    char viewport[64], framebuffer[64], frameRate[64], frameTime[64], sessionTime[64];
    std::snprintf(viewport, sizeof(viewport), "%.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
    // framebuffer size includes the pixel scale on high-DPI displays
    std::snprintf(framebuffer, sizeof(framebuffer), "%.0f x %.0f",
                  io.DisplaySize.x * io.DisplayFramebufferScale.x,
                  io.DisplaySize.y * io.DisplayFramebufferScale.y);
    std::snprintf(frameRate, sizeof(frameRate), "%.1f FPS", io.Framerate);
    std::snprintf(frameTime, sizeof(frameTime), "%.2f ms", io.Framerate > 0.f ? 1000.f / io.Framerate : 0.f);
    // ImGui's clock starts with the UI context, including time on the boot screens
    const int seconds = static_cast<int>(ImGui::GetTime());
    std::snprintf(sessionTime, sizeof(sessionTime), "%02d:%02d:%02d",
                  seconds / 3600, (seconds / 60) % 60, seconds % 60);

    sectionTitle("Live Display");
    ImGui::TextWrapped("Display values reflect this running application.");
    if (beginInfoTable("displayDetails")) {
        infoRow("Viewport", viewport);
        infoRow("Framebuffer", framebuffer);
        infoRow("Frame Rate", frameRate);
        infoRow("Average Frame", frameTime);
        infoRow("Session Time", sessionTime);
        ImGui::EndTable();
    }

    sectionTitle("Graphics Stack");
    if (beginInfoTable("graphicsDetails")) {
        infoRow("Window Library", "GLFW 3.4");
        infoRow("Graphics API", "OpenGL 3");
        infoRow("UI Library", "Dear ImGui");
        infoRow("ImGui Version", ImGui::GetVersion());
        ImGui::EndTable();
    }
}

void AppSysInfo::drawAbout() {
    sectionTitle("About CSOPESY OS");
    ImGui::TextWrapped("A desktop-style operating system mock-up for CSOPESY Semi-Major Output 2, built with C++17 and Dear ImGui.");
    ImGui::Spacing();
    ImGui::TextWrapped("Explore sample files, view system information, and monitor simulated processes through the taskbar.");

    // group names match README.md
    sectionTitle("Created By");
    ImGui::TextWrapped("CHIU, Kristopher Lance");
    ImGui::TextWrapped("KE, Xan Luo");
    ImGui::TextWrapped("RAMIREZ, Diana Angela");
    ImGui::TextWrapped("YAMSUAN, Rhian Claire");

    sectionTitle("Version");
    ImGui::Text("CSOPESY OS v1.0");
    ImGui::TextWrapped("Use the PWR button on the taskbar to shut down the system.");
}
