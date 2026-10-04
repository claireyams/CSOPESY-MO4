#pragma once
#include "OS.h"
#include "imgui.h"

// base layer of the OS. always rendered first every frame.
class Desktop {
public:
    void init();
    void render(OSState& os);

private:
    void drawWallpaper();
    void drawClock();
    void drawWatermark();   // the small green "CSOPESY OS v1.0 ..." text on the left

    unsigned int wallpaperTex = 0;   // 0 = no texture, use gradient
    int wallW = 0, wallH = 0;
};

// loads an image file into an opengl texture, returns 0 if it fails
unsigned int loadTexture(const char* path, int* outW, int* outH);
