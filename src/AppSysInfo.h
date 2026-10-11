#pragma once
#include "OS.h"

// tabbed system details for the mock OS and its running display
class AppSysInfo {
public:
    void render(OSState& os);

private:
    void drawGeneral();  // simulated OS, hardware, and storage details
    void drawDisplay();  // live display metrics and graphics stack
    void drawAbout();    // project description, group credits, and version
};
