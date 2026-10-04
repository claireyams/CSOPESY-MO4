#pragma once
// shared state that every module can see. keep it small, dont dump everything here.

enum class Stage {
    BIOS,       // the black "CSOPESY Megatrends" screen
    LOADING,    // logo + "Loading."
    DESKTOP,    // normal running state
    SHUTDOWN    // after PWR is pressed
};

struct OSState {
    Stage stage = Stage::BIOS;

    // which windows are open. taskbar flips these, each app reads its own.
    bool showTaskMgr = false;
    bool showFiles   = false;
    bool showSysInfo = false;

    // set by the PWR button only
    bool shutdownRequested = false;
};

// the window is only allowed to close when this is true (see main.cpp)
extern bool g_allowClose;
