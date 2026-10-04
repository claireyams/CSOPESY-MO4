#pragma once
#include "OS.h"

// handles the two boot screens from page 1 of the specs
class BootScreen {
public:
    void reset();
    // returns true when the stage should change
    void render(OSState& os);

private:
    void renderBios(OSState& os);
    void renderLoading(OSState& os);

    double stageStart = 0.0;
    bool started = false;
};
