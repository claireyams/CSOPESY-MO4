cmake_minimum_required(VERSION 3.16)
project(csopesy_os CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4)

# imgui has no cmakelists so we just download it and compile the files we need
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5)

FetchContent_MakeAvailable(glfw imgui)
find_package(OpenGL REQUIRED)

add_executable(csopesy_os
    src/main.cpp
    src/BootScreen.cpp
    src/Desktop.cpp
    src/Taskbar.cpp
    src/TaskManager.cpp
    src/AppFiles.cpp
    src/AppSysInfo.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

target_include_directories(csopesy_os PRIVATE
    src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends)

target_link_libraries(csopesy_os PRIVATE glfw OpenGL::GL)

# copy assets (wallpaper, fonts) next to the exe
add_custom_command(TARGET csopesy_os POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:csopesy_os>/assets)
