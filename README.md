# [CSOPESY] - MO4 Desktop-Style OS Mock-up  
> Authors: CHIU, Kristopher Lance, KE, Xan Luo, RAMIREZ, Diana Angela, YAMSUAN, Rhian Claire

CSOPESY OS is a desktop-style operating system mock-up built for CSOPESY Semi-Major Output 2. It uses C++17 with GLFW, OpenGL 3, and Dear ImGui, and simulates the look of a minimal OS shell. On launch it plays a retro BIOS boot sequence and a loading screen, then goes to a desktop with a wallpaper and a real-time clock. A taskbar at the bottom opens the Files and System Info apps and a Task Manager showing placeholder processes with dummy CPU and memory values. The system is shut down with the PWR button, which plays a shutdown screen before closing. The window's close button is disabled on purpose.

The entry file where the main function is located is `src/main.cpp`.

## Build and run (command line)

CMake 3.16+ and a C++ compiler with support for C++17 (ex. g++) is required.

Initialise once:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
```
Use MinGW for Windows, otherwise you may omit

Build and run:

```powershell
cmake --build build
.\csopesy_os.exe
```
## Build and run (Visual Studio Code)

1. Install these extensions: **C/C++** and **CMake Tools** (both by Microsoft).
2. Open the project folder with **File > Open Folder** (the folder that contains `CMakeLists.txt`).
3. When prompted to configure the project, click **Yes**. Otherwise press `Ctrl+Shift+P` and run **CMake: Configure**.
4. Press `Ctrl+Shift+P`, run **CMake: Select a Kit**, and choose your GCC / MinGW kit (for example `GCC x86_64-w64-mingw32`).
5. Wait for the configure step to finish. It downloads GLFW and Dear ImGui, so it needs internet the first time.
6. In the bottom status bar, set the build target to `csopesy_os`.
7. Click **Build** in the status bar (or press `F7`).
8. Click the **Debug** (bug) icon or press `Ctrl+Shift+P` and run **CMake: Debug** to launch with the debugger. The **Launch** (play) icon runs it without the debugger.
9. To close the program, use the **PWR** button on the taskbar. The window's X button is disabled on purpose.


For contributing: Please make sure to add any additional `.cpp` files to `add_executables` in `CMakeLists.txt`. The build command stays the same regardless.