CSOPESY Semi-Major Output 2 - Desktop-Style OS Mock-up
Group: (names here)
Section: (section here)

Entry file: src/main.cpp  (main() is here)

Stack: C++17, GLFW 3.4, OpenGL 3, Dear ImGui v1.91.5 (all pulled by CMake)

How to run
  1. Install CMake (3.16+) and a C++ compiler (Visual Studio 2022 / g++)
  2. mkdir build && cd build
  3. cmake ..
  4. cmake --build . --config Release
  5. run csopesy_os (the assets folder is copied next to the exe)
  Visual Studio: File > Open > Folder, pick this folder, select csopesy_os, press Run.
  First configure needs internet because it downloads glfw and imgui.

How to close: use the PWR button on the taskbar. The window X button is
disabled on purpose (spec).

Frame order: boot screens -> desktop -> app windows -> taskbar

Who does what (edit if needed, search "TODO [Mx]" to find your parts)
  M1 - BootScreen.cpp + main.cpp TODOs (bios fun fact, logo, loading, font, args)
  M2 - Desktop.cpp (wallpaper, watermark, clock) + shutdown screen + PWR popup
  M3 - Taskbar.cpp (icons, tray) + TaskManager.cpp (fake processes, table)
  M4 - AppFiles.cpp + AppSysInfo.cpp, plus PPT report and video

Requirement checklist
  [ ] Wallpaper fills window
  [ ] Real-time clock in a corner
  [ ] PWR button shuts down, X button does not close
  [ ] Taskbar has 3+ icon buttons
  [ ] 2 buttons open unique screens
  [ ] 1 button opens Task Manager
  [ ] Task Manager table: process, cpu, memory (dummy values ok)

Reminder for the quiz: no recompiling and no code edits while recording,
only change parameters. Record the IDE pressing Run/Debug in one uncut video
(480p-720p, max 1GB).
