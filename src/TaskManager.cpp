#include "TaskManager.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <cstdlib>

void TaskManager::init() {
    // TODO [M3]: fill 'procs' with 8-12 dummy processes. dummy values are fine.
    //   example: procs.push_back({"System", 4, 1.2f, 120.f});
    //   use names that make sense for this OS (csopesy_shell.exe, taskbar.exe ...)
}

void TaskManager::updateFakeValues() {
    // TODO [M3]: every ~1 sec nudge each process' cpu and mem by a small random
    //   amount (keep cpu between 0 and 100, mem never negative).
    //   use glfwGetTime() and lastUpdate for the timing.
    //   (static numbers are accepted by the spec but moving ones look better)
}

void TaskManager::render(OSState& os) {
    if (!os.showTaskMgr) return;

    ImGui::SetNextWindowSize(ImVec2(560, 380), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(120, 80), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Task Manager", &os.showTaskMgr)) {
        updateFakeValues();
        drawSummaryBar();
        ImGui::Separator();
        drawProcessTable();
    }
    ImGui::End();
}

void TaskManager::drawSummaryBar() {
    // TODO [M3]: show total CPU % and total memory used, add them up from 'procs'.
    //            ImGui::ProgressBar looks close to the windows one.
}

void TaskManager::drawProcessTable() {
    // columns required by spec: Process, CPU, Memory. PID is extra.
    ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable |
                            ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("procs", 4, flags)) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("PID");
        ImGui::TableSetupColumn("CPU %");
        ImGui::TableSetupColumn("Memory (MB)");
        ImGui::TableHeadersRow();

        // TODO [M3]: loop through 'procs' and draw one row each
        //   - ImGui::TableNextRow(); then TableSetColumnIndex(i) + Text for each col
        //   - color the CPU cell red-ish if it's high (>50)
        //   - (optional) handle sorting using ImGui::TableGetSortSpecs()
        //   - (optional) right click a row -> "End task" removes it from the vector

        ImGui::EndTable();
    }
}
