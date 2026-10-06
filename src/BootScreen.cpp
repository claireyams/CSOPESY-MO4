#include "BootScreen.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>

ImFont* g_monoFont = nullptr;

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

// shown only after every line above is done
static const char* kFunFacts[] = {
    "Fun Fact: The CPU specs is a tribute to Bemani Python 2 Engine, the OS that powers up Dance Dance Revolution SuperNova series.",
    "It was essentially a retail Sony PlayStation 2 (SCPH-50000) inside a metal box with a specialized I/O board!",
};
static const int kFunCount = sizeof(kFunFacts) / sizeof(kFunFacts[0]);

// ascii logo (one raw string per row so no backslash escaping is needed)
static const char* kLogo[] = {
    R"LOGO(  ____  ____    ___   ____   _____  ____  __   __)LOGO",
    R"LOGO( / ___|/ ___|  / _ \ |  _ \ | ____|/ ___| \ \ / /)LOGO",
    R"LOGO(| |    \___ \ | | | || |_) ||  _|  \___ \  \ V / )LOGO",
    R"LOGO(| |___  ___) || |_| ||  __/ | |___  ___) |  | |  )LOGO",
    R"LOGO( \____||____/  \___/ |_|    |_____||____/   |_|  )LOGO",
};
static const int kLogoCount = sizeof(kLogo) / sizeof(kLogo[0]);

static const char* kCredits[] = {
    "CSOPESY Operating System Emulator v1.0",
    "Created By: Dr. Neil Patrick Del Gallego",
    "",
    "Part of Project Anito:",
    "Support for the PH Gaming Industry Towards Embracing",
    "In-House Game Engine Development",
};
static const int kCreditCount = sizeof(kCredits) / sizeof(kCredits[0]);

// helper: fullscreen borderless imgui window, used by every screen
static void beginFullscreen(const char* id, ImU32 bg) {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
    ImGui::Begin(id, nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
    if (g_monoFont) {
#if IMGUI_VERSION_NUM >= 19200
        ImGui::PushFont(g_monoFont, 0.0f);   // 0 = keep the size the font was loaded with
#else
        ImGui::PushFont(g_monoFont);
#endif
    }
}
static void endFullscreen() {
    if (g_monoFont) ImGui::PopFont();
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// helper: draw one line of text centered horizontally in the current window
static void centeredText(const char* s, const ImVec4& col) {
    float w = ImGui::CalcTextSize(s).x;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - w) * 0.5f);
    ImGui::TextColored(col, "%s", s);
}

void BootScreen::reset() {
    started = false;
}

void BootScreen::configure(float biosLineDelay, float loadingDur) {
    if (biosLineDelay > 0.f)  lineDelay = biosLineDelay;
    if (loadingDur >= 0.f)    loadingDuration = loadingDur;
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
    const ImVec4 textCol(0.9f, 0.9f, 0.9f, 1.f);

    beginFullscreen("##bios", IM_COL32(0, 0, 0, 255));

    // show one more line every 'lineDelay' seconds so it looks like its booting.
    // the fun fact lines count as extra lines after the normal ones.
    int total = kBiosCount + kFunCount;
    int shown = (int)(t / lineDelay);
    if (shown > total) shown = total;

    int biosShown = shown < kBiosCount ? shown : kBiosCount;
    for (int i = 0; i < biosShown; i++) {
        ImGui::TextColored(textCol, "%s", kBiosLines[i]);
    }

    // fun fact only appears once all the lines above are done
    if (shown > kBiosCount) {
        ImGui::NewLine();   // blank line like the screenshot
        for (int i = 0; i < shown - kBiosCount; i++) {
            ImGui::PushStyleColor(ImGuiCol_Text, textCol);
            ImGui::TextWrapped("%s", kFunFacts[i]);
            ImGui::PopStyleColor();
        }
    }

    // blinking cursor on the line after the last one (0.5s on / 0.5s off)
    bool blinkOn = std::fmod(t, 1.0) < 0.5;
    ImGui::TextColored(textCol, "%s", blinkOn ? "_" : " ");

    endFullscreen();

    // move on once everything is shown + a short pause so it can be read
    if (t > total * lineDelay + biosHold) {
        os.stage = Stage::LOADING;
        started = false;
    }
}

void BootScreen::renderLoading(OSState& os) {
    double t = glfwGetTime() - stageStart;
    ImGuiViewport* vp = ImGui::GetMainViewport();

    const ImVec4 cyan(0.20f, 0.85f, 0.95f, 1.f);
    const ImVec4 white(0.92f, 0.92f, 0.92f, 1.f);
    const ImVec4 green(0.25f, 0.95f, 0.35f, 1.f);

    beginFullscreen("##loading", IM_COL32(0, 0, 0, 255));

    // logo, centered, roughly in the upper third like the screenshot
    ImGui::SetCursorPosY(vp->Size.y * 0.20f);
    for (int i = 0; i < kLogoCount; i++) centeredText(kLogo[i], cyan);

    ImGui::NewLine();

    // credits under the logo
    for (int i = 0; i < kCreditCount; i++) centeredText(kCredits[i], white);

    // "Loading." with 1-3 dots cycling every 0.4s. position is based on the
    // widest version ("Loading...") so the text doesnt jump around.
    int dots = 1 + (int)(t / 0.4) % 3;
    char buf[16];
    snprintf(buf, sizeof(buf), "Loading%.*s", dots, "...");
    float fullW = ImGui::CalcTextSize("Loading...").x;
    ImGui::SetCursorPosY(vp->Size.y - 80.f);
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - fullW) * 0.5f);
    ImGui::TextColored(green, "%s", buf);

    endFullscreen();

    if (t > loadingDuration) {
        os.stage = Stage::DESKTOP;
    }
}

void BootScreen::renderShutdown(double elapsed) {
    beginFullscreen("##shutdown", IM_COL32(0, 0, 0, 255));

    // "Shutting down" + 0-3 dots, same no-jumping trick as the loading text
    int dots = (int)(elapsed / 0.35) % 4;
    char buf[32];
    snprintf(buf, sizeof(buf), "Shutting down%.*s", dots, "...");
    ImVec2 full = ImGui::CalcTextSize("Shutting down...");
    ImGui::SetCursorPos(ImVec2((ImGui::GetWindowWidth() - full.x) * 0.5f,
                               (ImGui::GetWindowHeight() - full.y) * 0.5f));
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.f), "%s", buf);

    endFullscreen();
}