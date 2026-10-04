#pragma once
#include "OS.h"
#include <string>
#include <vector>

struct FakeProcess {
    std::string name;
    int   pid;
    float cpu;      // percent
    float memMB;
};

class TaskManager {
public:
    void init();
    void render(OSState& os);

private:
    void updateFakeValues();    // so the numbers move a bit
    void drawProcessTable();
    void drawSummaryBar();      // total cpu / mem at the top

    std::vector<FakeProcess> procs;
    double lastUpdate = 0.0;
};
