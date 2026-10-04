#pragma once
#include "OS.h"

// fixed bar at the bottom of the screen
class Taskbar {
public:
    static constexpr float HEIGHT = 56.f;
    void render(OSState& os);

private:
    // returns true if clicked. 'active' = the app window is currently open
    bool iconButton(const char* id, const char* label, bool active);
    void drawPwrButton(OSState& os);
    void drawTray();   // VOL / NET on the right side
};
