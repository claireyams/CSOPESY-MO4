// CSOPESY Semi-Major Output 2 - Desktop-Style OS Mock-up
// entry point: this file has main()
//
// frame order:  boot screens -> desktop -> app windows -> taskbar
// (taskbar is drawn last so it stays on top of everything)
//
// command line args (all optional, so the quiz can change them without recompiling):
//   --width N          window width            (default 1280)
//   --height N         window height           (default 720)
//   --fullscreen       fullscreen on the primary monitor (ignores width/height)
//   --font PATH        monospace .ttf for the boot screens
//   --font-size N      size of that font       (default 16)
//   --bios-delay S     seconds between BIOS lines (default 0.2)
//   --loading-time S   seconds on the loading screen (default 4.0)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "OS.h"
#include "BootScreen.h"
#include "Desktop.h"
#include "Taskbar.h"
#include "TaskManager.h"
#include "AppFiles.h"
#include "AppSysInfo.h"

bool g_allowClose = false;

// config / args

struct Config {
    int         width       = 1280;
    int         height      = 720;
    bool        fullscreen  = false;
    const char* fontPath    = nullptr;
    float       fontSize    = 16.f;
    float       biosDelay   = 0.2f;
    float       loadingTime = 4.0f;
};

// returns the value after 'name' (e.g. --width 1920), or nullptr if not given
static const char* getArg(int argc, char** argv, const char* name) {
    for (int i = 1; i + 1 < argc; i++) {
        if (strcmp(argv[i], name) == 0) return argv[i + 1];
    }
    return nullptr;
}

static bool hasFlag(int argc, char** argv, const char* name) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], name) == 0) return true;
    }
    return false;
}

static Config parseArgs(int argc, char** argv) {
    Config c;
    if (const char* v = getArg(argc, argv, "--width"))        c.width  = atoi(v);
    if (const char* v = getArg(argc, argv, "--height"))       c.height = atoi(v);
    if (const char* v = getArg(argc, argv, "--font"))         c.fontPath = v;
    if (const char* v = getArg(argc, argv, "--font-size"))    c.fontSize = (float)atof(v);
    if (const char* v = getArg(argc, argv, "--bios-delay"))   c.biosDelay = (float)atof(v);
    if (const char* v = getArg(argc, argv, "--loading-time")) c.loadingTime = (float)atof(v);
    c.fullscreen = hasFlag(argc, argv, "--fullscreen");

    // basic sanity so a typo in the quiz doesnt give a 0x0 window
    if (c.width  < 640) c.width  = 640;
    if (c.height < 360) c.height = 360;
    if (c.fontSize < 8.f) c.fontSize = 8.f;
    return c;
}

// font

static bool fileExists(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    fclose(f);
    return true;
}

// loads a monospace font for the boot screens. tries the --font path first, then
// assets/ (a few relative paths, since the IDE working directory can differ),
// then common system fonts. returns nullptr if nothing is found -> default font.
static ImFont* loadMonoFont(const Config& cfg) {
    ImGuiIO& io = ImGui::GetIO();

    // The first font added becomes the default for the whole UI,
    io.Fonts->AddFontDefault();

    std::vector<std::string> candidates;
    if (cfg.fontPath) candidates.push_back(cfg.fontPath);
    candidates.push_back("assets/mono.ttf");
    candidates.push_back("../assets/mono.ttf");
    candidates.push_back("../../assets/mono.ttf");
    candidates.push_back("C:/Windows/Fonts/consola.ttf");
    candidates.push_back("C:/Windows/Fonts/cour.ttf");
    candidates.push_back("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
    candidates.push_back("/System/Library/Fonts/Menlo.ttc");

    for (const std::string& path : candidates) {
        if (fileExists(path.c_str())) {
            ImFont* f = io.Fonts->AddFontFromFileTTF(path.c_str(), cfg.fontSize);
            if (f) return f;
        }
    }
    fprintf(stderr, "No monospace font found, using the default imgui font.\n");
    return nullptr;
}

// glfw / style

static void onGlfwError(int code, const char* msg) {
    fprintf(stderr, "GLFW error %d: %s\n", code, msg);
}

static void onCloseAttempt(GLFWwindow* w) {
    if (!g_allowClose) glfwSetWindowShouldClose(w, GLFW_FALSE);
}

static void applyStyle() {
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 6.f;
    s.FrameRounding  = 4.f;
}

// main

int main(int argc, char** argv) {
    Config cfg = parseArgs(argc, argv);

    glfwSetErrorCallback(onGlfwError);
    if (!glfwInit()) return 1;

    const char* glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // fullscreen uses the primary monitor's current video mode
    GLFWmonitor* monitor = nullptr;
    int winW = cfg.width, winH = cfg.height;
    if (cfg.fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
        if (mode) { winW = mode->width; winH = mode->height; }
        else      { monitor = nullptr; }   
    }

    GLFWwindow* window = glfwCreateWindow(winW, winH, "CSOPESY Desktop OS Emulator", monitor, nullptr);
    if (!window) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetWindowCloseCallback(window, onCloseAttempt);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;  
    applyStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // monospace font for the retro boot screens (see loadMonoFont above)
    g_monoFont = loadMonoFont(cfg);

    OSState     os;
    BootScreen  boot;
    Desktop     desktop;
    Taskbar     taskbar;
    TaskManager taskMgr;
    AppFiles    appFiles;
    AppSysInfo  appSysInfo;

    boot.configure(cfg.biosDelay, cfg.loadingTime);
    desktop.init();
    taskMgr.init();

    double shutdownStart = 0.0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        switch (os.stage) {
        case Stage::BIOS:
        case Stage::LOADING:
            boot.render(os);
            break;

        case Stage::DESKTOP:
            desktop.render(os);          // base layer first
            appFiles.render(os);
            appSysInfo.render(os);
            taskMgr.render(os);
            taskbar.render(os);          // always on top

            if (os.shutdownRequested) {
                os.stage = Stage::SHUTDOWN;
                shutdownStart = glfwGetTime();
            }
            break;

        case Stage::SHUTDOWN:
            boot.renderShutdown(glfwGetTime() - shutdownStart);
            if (glfwGetTime() - shutdownStart > 2.0) {
                g_allowClose = true;
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            break;
        }

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}