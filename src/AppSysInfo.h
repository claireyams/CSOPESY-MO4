#pragma once
#include "OS.h"

// tabbed system details for the mock OS and its running display
class AppSysInfo {
public:
    void render(OSState& os);

private:
    void drawGeneral();
    void drawDisplay();
    void drawAbout();
};
