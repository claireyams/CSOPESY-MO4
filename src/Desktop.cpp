#include "Desktop.h"
#include <GLFW/glfw3.h>
#include <ctime>
#include <cstdio>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include "stb_image.h"

// windows gl.h is stuck on gl 1.1 so this one is missing
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

unsigned int loadTexture(const char* path, int* outW, int* outH) {
    int w = 0, h = 0, channels = 0;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 4);   // force rgba
    if (!data) return 0;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    if (outW) *outW = w;
    if (outH) *outH = h;
    return (unsigned int)tex;
}

// ---- procedural wallpaper helpers (used when there is no image) ----

static void drawCloud(ImDrawList* dl, float cx, float cy, float s) {
    ImU32 white = IM_COL32(255, 255, 255, 200);
    dl->AddCircleFilled(ImVec2(cx,            cy),            18.f * s, white, 20);
    dl->AddCircleFilled(ImVec2(cx + 20.f * s, cy - 8.f * s),  24.f * s, white, 20);
    dl->AddCircleFilled(ImVec2(cx + 46.f * s, cy - 2.f * s),  20.f * s, white, 20);
    dl->AddCircleFilled(ImVec2(cx + 66.f * s, cy + 4.f * s),  14.f * s, white, 20);
    dl->AddRectFilled(ImVec2(cx, cy), ImVec2(cx + 66.f * s, cy + 18.f * s), white, 8.f * s);
}

// a hill is drawn as thin vertical strips under a sine curve
static void drawHill(ImDrawList* dl, ImVec2 a, ImVec2 b, float baseY, float amp,
                     float freq, float phase, ImU32 top, ImU32 bottom) {
    const float step = 4.f;
    float width = b.x - a.x;
    for (float x = a.x; x < b.x; x += step) {
        float t = (x - a.x) / width;
        float y = baseY - amp * sinf(t * freq * 3.14159f + phase);
        dl->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + step + 1.f, b.y),
                                    top, top, bottom, bottom);
    }
}

static void drawProceduralWallpaper(ImDrawList* dl, ImVec2 a, ImVec2 b) {
    float W = b.x - a.x;
    float H = b.y - a.y;
    float t = (float)ImGui::GetTime();

    // sky
    ImU32 skyTop = IM_COL32(60, 130, 225, 255);
    ImU32 skyLow = IM_COL32(175, 215, 250, 255);
    dl->AddRectFilledMultiColor(a, b, skyTop, skyTop, skyLow, skyLow);

    // clouds drift to the right and wrap around
    struct CloudInfo { float startX, y, scale, speed; };
    static const CloudInfo clouds[] = {
        {0.10f, 0.12f, 1.4f, 9.f},
        {0.45f, 0.25f, 1.0f, 6.f},
        {0.70f, 0.10f, 1.7f, 11.f},
        {0.90f, 0.32f, 0.8f, 5.f},
        {0.30f, 0.38f, 1.1f, 7.f},
    };
    for (const CloudInfo& c : clouds) {
        float span = W + 300.f;
        float x = fmodf(c.startX * W + t * c.speed, span) - 150.f;
        drawCloud(dl, a.x + x, a.y + c.y * H, c.scale);
    }

    // hills, far one first so the near one covers it
    drawHill(dl, a, b, a.y + H * 0.62f, H * 0.05f, 1.6f, 0.4f,
             IM_COL32(95, 175, 70, 255), IM_COL32(50, 125, 40, 255));
    drawHill(dl, a, b, a.y + H * 0.72f, H * 0.07f, 1.1f, 2.2f,
             IM_COL32(70, 160, 40, 255), IM_COL32(25, 95, 20, 255));
}

// ---- Desktop ----

void Desktop::init() {
    // the working dir changes depending on how you launch (vs vs terminal)
    // so try a few places. if none work we just use the gradient scene.
    const char* paths[] = {
        "assets/wallpaper.png",
        "assets/wallpaper.jpg",
        "../assets/wallpaper.png",
        "../assets/wallpaper.jpg",
    };
    for (const char* p : paths) {
        wallpaperTex = loadTexture(p, &wallW, &wallH);
        if (wallpaperTex != 0) break;
    }
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

    ImGui::End();
}

void Desktop::drawWallpaper() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 a = vp->Pos;
    ImVec2 b = ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);

    if (wallpaperTex != 0 && wallW > 0 && wallH > 0) {
        // "cover" fit: fill the whole window and crop the extra part of the
        // image so it doesnt get stretched
        float winAspect = vp->Size.x / vp->Size.y;
        float imgAspect = (float)wallW / (float)wallH;
        ImVec2 uv0(0.f, 0.f), uv1(1.f, 1.f);

        if (imgAspect > winAspect) {
            float visible = winAspect / imgAspect;
            uv0.x = (1.f - visible) * 0.5f;
            uv1.x = 1.f - uv0.x;
        } else {
            float visible = imgAspect / winAspect;
            uv0.y = (1.f - visible) * 0.5f;
            uv1.y = 1.f - uv0.y;
        }
        dl->AddImage((ImTextureID)(intptr_t)wallpaperTex, a, b, uv0, uv1);
        return;
    }

    drawProceduralWallpaper(dl, a, b);
}

void Desktop::drawWatermark() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    const char* text = "CSOPESY OS v1.0 - System Online";
    ImVec2 ts = ImGui::CalcTextSize(text);
    ImVec2 pos = ImVec2(vp->Pos.x + 14.f, vp->Pos.y + vp->Size.y * 0.5f - ts.y * 0.5f);

    // dark shadow first so the green is readable on any wallpaper
    dl->AddText(ImVec2(pos.x + 1.f, pos.y + 1.f), IM_COL32(0, 0, 0, 200), text);
    dl->AddText(pos, IM_COL32(60, 255, 90, 255), text);
}

void Desktop::drawClock() {
    // read the time every call so it updates every frame
    time_t now = time(nullptr);
    tm lt;
#ifdef _WIN32
    localtime_s(&lt, &now);
#else
    localtime_r(&now, &lt);
#endif
    char buf[64];
    strftime(buf, sizeof(buf), "%A, %b %d, %Y | %I:%M:%S %p", &lt);

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImVec2 ts = ImGui::CalcTextSize(buf);
    float pad = 8.f;
    ImVec2 br = ImVec2(vp->Pos.x + vp->Size.x - 10.f, vp->Pos.y + 10.f + ts.y + pad * 2);
    ImVec2 tl = ImVec2(br.x - ts.x - pad * 2, vp->Pos.y + 10.f);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(tl, br, IM_COL32(0, 0, 0, 170), 4.f);
    dl->AddText(ImVec2(tl.x + pad, tl.y + pad), IM_COL32(255, 255, 255, 255), buf);
}