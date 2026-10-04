// CSOPESY Semi-Major Output 2 - Desktop-Style OS Mock-up
// entry point: this file has main()
//
// frame order:  boot screens -> desktop -> app windows -> taskbar
// (taskbar is drawn last so it stays on top of everything)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>

#include "OS.h"
#include "BootScreen.h"
#include "Desktop.h"
#include "Taskbar.h"
#include "TaskManager.h"
#include "AppFiles.h"
#include "AppSysInfo.h"

bool g_allowClose = false;

static void onGlfwError(int code, const char* msg) {
    fprintf(stderr, "GLFW error %d: %s\n", code, msg);
}

// the spec says the app must be closed with PWR, so we cancel the X button / alt+f4.
// if you need to kill it while debugging, set g_allowClose = true here temporarily.
static void onCloseAttempt(GLFWwindow* w) {
    if (!g_allowClose) glfwSetWindowShouldClose(w, GLFW_FALSE);
}

static void applyStyle() {
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 6.f;
    s.FrameRounding  = 4.f;
    // TODO [M2]: tweak colors so it looks like our own OS, not default imgui
    //            (title bar color, button colors, etc)
}

int main() {
    glfwSetErrorCallback(onGlfwError);
    if (!glfwInit()) return 1;

    const char* glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // TODO [M1]: window size/fullscreen can be read from command line args so
    //            we can change them during the quiz without recompiling
    GLFWwindow* window = glfwCreateWindow(1280, 720, "CSOPESY Desktop OS Emulator", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetWindowCloseCallback(window, onCloseAttempt);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;   // dont save window positions between runs
    applyStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // TODO [M1]: load a monospace font with io.Fonts->AddFontFromFileTTF so the
    //            bios screen looks retro (put the .ttf in assets/)

    OSState     os;
    BootScreen  boot;
    Desktop     desktop;
    Taskbar     taskbar;
    TaskManager taskMgr;
    AppFiles    appFiles;
    AppSysInfo  appSysInfo;

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
            // TODO [M2]: draw a black fullscreen "Shutting down..." screen
            //            (same trick as the boot screens, see beginFullscreen in BootScreen.cpp)
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
