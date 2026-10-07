#pragma once
#include "OS.h"
#include <string>
#include <vector>

struct FakeProcess {
    std::string name;
    int   pid;
    float cpu;
    float memMB;
};

class TaskManager {
public:
    void init();
    void render(OSState& os);

private:
    void updateFakeValues();
    void drawProcessTable();
    void drawSummaryBar();

    std::vector<FakeProcess> procs;
    double lastUpdate = 0.0;
};
