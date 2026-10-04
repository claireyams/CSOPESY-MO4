#include "Desktop.h"
#include <GLFW/glfw3.h>
#include <ctime>
#include <cstdio>

unsigned int loadTexture(const char* path, int* outW, int* outH) {
    // TODO [M2]: (optional, only if you want an image wallpaper)
    //   1. download stb_image.h into src/ and add
    //        #define STB_IMAGE_IMPLEMENTATION
    //        #include "stb_image.h"
    //   2. stbi_load the file as RGBA
    //   3. glGenTextures / glBindTexture / glTexParameteri (linear + clamp)
    //   4. glTexImage2D, then free the image data
    //   5. fill outW and outH and return the texture id
    // the gradient wallpaper already works so this can be skipped.
    (void)path; (void)outW; (void)outH;
    return 0;
}

void Desktop::init() {
    // TODO [M2]: call loadTexture("assets/wallpaper.png", &wallW, &wallH)
    //            and store the result in wallpaperTex
}

void Desktop::render(OSState& os) {
    (void)os;
    ImGuiViewport* vp = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::Begin("##desktop", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoBackground);

    drawWallpaper();
    drawWatermark();
    drawClock();

    // TODO [M2]: (optional) desktop icons on the left side, double click opens an app

    ImGui::End();
}

void Desktop::drawWallpaper() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 a = vp->Pos;
    ImVec2 b = ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);

    if (wallpaperTex != 0) {
        // TODO [M2]: draw the texture stretched over the whole window
        //            dl->AddImage((ImTextureID)(intptr_t)wallpaperTex, a, b);
        //            keep the aspect ratio if you can
        return;
    }

    // fallback: sky to grass gradient
    ImU32 sky   = IM_COL32(70, 140, 230, 255);
    ImU32 horiz = IM_COL32(170, 210, 250, 255);
    ImU32 grass = IM_COL32(60, 150, 40, 255);
    float mid = a.y + (b.y - a.y) * 0.6f;
    dl->AddRectFilledMultiColor(a, ImVec2(b.x, mid), sky, sky, horiz, horiz);
    dl->AddRectFilledMultiColor(ImVec2(a.x, mid), b, grass, grass,
                                IM_COL32(30, 100, 20, 255), IM_COL32(30, 100, 20, 255));

    // TODO [M2]: make it nicer. ideas: a few clouds with AddCircleFilled,
    //            a hill using AddBezierCubic or AddConvexPolyFilled,
    //            slow animation using ImGui::GetTime()
}

void Desktop::drawWatermark() {
    // TODO [M2]: small green text at the left side, vertically centered
    //            "CSOPESY OS v1.0 - System Online"
    //            use dl->AddText
}

void Desktop::drawClock() {
    // must update every frame, so we just read the time each call
    time_t now = time(nullptr);
    tm* lt = localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%A, %b %d, %Y | %I:%M %p", lt);

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImVec2 ts = ImGui::CalcTextSize(buf);
    float pad = 8.f;
    ImVec2 br = ImVec2(vp->Pos.x + vp->Size.x - 10.f, vp->Pos.y + 10.f + ts.y + pad * 2);
    ImVec2 tl = ImVec2(br.x - ts.x - pad * 2, vp->Pos.y + 10.f);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(tl, br, IM_COL32(0, 0, 0, 170), 4.f);
    dl->AddText(ImVec2(tl.x + pad, tl.y + pad), IM_COL32(255, 255, 255, 255), buf);

    // TODO [M2]: (optional) show seconds so its obvious its updating live
}
