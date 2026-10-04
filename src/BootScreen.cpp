#include "BootScreen.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

// text for the bios screen, taken from the spec screenshot
static const char* kBiosLines[] = {
    "CSOPESY Megatrends",
    "Released: 12/01/94",
    "DLSU GAME Lab (C)1994 CSOPESY Megatrends Inc.",
    "",
    "Memory Test:",
    "Checking RAM : 64000K OK",
    "",
    "CPU Type: Pentium III",
    "BIOS Version: BCN SIT 1989-1994 Special CSOPESY",
    "Main Processor: Pentium III",
    "Numeric Processor: Built-in",
    "",
    "Primary Master:   420 MB WDCAC2420H",
    "Primary Slave:    None",
    "Secondary Master: CD-ROM LTN-385A",
    "Secondary Slave:  None",
};
static const int kBiosCount = sizeof(kBiosLines) / sizeof(kBiosLines[0]);

// helper: fullscreen borderless imgui window, used by every screen
static void beginFullscreen(const char* id, ImU32 bg) {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);
    ImGui::Begin(id, nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
}
static void endFullscreen() {
    ImGui::End();
    ImGui::PopStyleColor();
}

void BootScreen::reset() {
    started = false;
}

void BootScreen::render(OSState& os) {
    if (!started) {
        stageStart = glfwGetTime();
        started = true;
    }
    if (os.stage == Stage::BIOS)        renderBios(os);
    else if (os.stage == Stage::LOADING) renderLoading(os);
}

void BootScreen::renderBios(OSState& os) {
    double t = glfwGetTime() - stageStart;

    beginFullscreen("##bios", IM_COL32(0, 0, 0, 255));

    // show one more line every 0.2s so it looks like its booting
    int shown = (int)(t / 0.2);
    if (shown > kBiosCount) shown = kBiosCount;
    for (int i = 0; i < shown; i++) {
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.f), "%s", kBiosLines[i]);
    }

    // TODO [M1]: add the "Fun Fact" lines at the bottom of the screenshot
    //            (only show them after all the lines above are done)
    // TODO [M1]: optional blinking cursor at the end of the last line

    endFullscreen();

    // move on after ~5s
    if (t > 5.0) {
        os.stage = Stage::LOADING;
        started = false;
    }
}

void BootScreen::renderLoading(OSState& os) {
    double t = glfwGetTime() - stageStart;

    beginFullscreen("##loading", IM_COL32(0, 0, 0, 255));

    // TODO [M1]: draw the ascii CSOPESY logo centered, cyan-ish color.
    //            can use ImGui::CalcTextSize to center each line.
    //            (or use a drawn logo / texture if you want, up to you)

    // TODO [M1]: credits text under the logo:
    //            "CSOPESY Operating System Emulator v1.0"
    //            "Created By: Dr. Neil Patrick Del Gallego"
    //            "Part of Project Anito: ..."  (copy from spec page 1)

    // TODO [M1]: green "Loading." text near the bottom with dots that
    //            cycle (. .. ...) based on t

    endFullscreen();

    if (t > 4.0) {
        os.stage = Stage::DESKTOP;
    }
}
