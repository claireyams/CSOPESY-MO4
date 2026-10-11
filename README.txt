CSOPESY Semi-Major Output 2 - Desktop-Style OS Mock-up
Group: CHIU, Kristopher Lance; KE, Xan Luo; RAMIREZ, Diana Angela; YAMSUAN, Rhian Claire
Section: S02

Entry file: src/main.cpp  (main() is here)

Stack: C++17, GLFW 3.4, OpenGL 3, Dear ImGui v1.91.5 (all pulled by CMake)

Requirements
  CMake 3.16+ and a C++17 compiler (g++ / MinGW or Visual Studio 2022).
  The first configure needs internet, since it downloads GLFW and Dear ImGui.

How to run (command line, MinGW / g++)
  1. cmake -S . -B build -G "MinGW Makefiles"
  2. cmake --build build
  3. run csopesy_os.exe   (<-- put the real path here)

How to run (Visual Studio 2022)
  File > Open > Folder, select this folder, choose csopesy_os, press Run.

How to close
  Use the PWR button on the taskbar. The window's X button is disabled
  on purpose, as required by the spec.

Optional parameters (no recompiling needed):
  --width N          window width (default 1280)
  --height N         window height (default 720)
  --fullscreen       fullscreen on the primary monitor
  --font PATH        monospace .ttf for the boot screens
  --font-size N      boot screen font size (default 16)
  --bios-delay S     seconds between BIOS lines (default 0.2)
  --loading-time S   seconds on the loading screen (default 4.0)

  Example: csopesy_os.exe --width 1920 --height 1080 --loading-time 6