#pragma once
#include "OS.h"

struct ImFont;

// monospace font used by the boot + shutdown screens. set in main.cpp,
// stays nullptr if no font file was found (then the default imgui font is used)
extern ImFont* g_monoFont;

// handles the two boot screens from page 1 of the specs, plus the shutdown screen
class BootScreen {
public:
    void reset();
    void configure(float biosLineDelay, float loadingDuration);
    void render(OSState& os);
    void renderShutdown(double elapsed);

private:
    void renderBios(OSState& os);
    void renderLoading(OSState& os);

    double stageStart = 0.0;
    bool started = false;

    float lineDelay = 0.2f;        // seconds per BIOS line
    float biosHold = 1.5f;         // pause after the last line before moving on
    float loadingDuration = 4.0f;  // seconds on the loading screen
};