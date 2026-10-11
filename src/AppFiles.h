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
    bool drawFileTable();  // true = open the selected item after leaving the child panels
    void drawStatusBar();
    void drawPreview();

    // member state survives between frames and when the window is reopened
    int currentFolder = 0;        // 0 = Local Disk (C:)
    int selectedEntry = -1;       // -1 = nothing selected
    int previewEntry = -1;        // -1 = no file preview
    bool resetFileScroll = true;  // scroll back to the start after changing folders
};
