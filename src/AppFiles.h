#pragma once
#include "OS.h"

// an in-memory file explorer; none of the entries touch the host filesystem
class AppFiles {
public:
    void render(OSState& os);

private:
    void navigateTo(int folder);
    void openSelected();
    void drawToolbar();
    void drawSidebar();
    bool drawFileTable();
    void drawStatusBar();
    void drawPreview();

    int currentFolder = 0;
    int selectedEntry = -1;
    int previewEntry = -1;
    bool resetFileScroll = true;
};
