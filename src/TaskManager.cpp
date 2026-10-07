#include "TaskManager.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdlib>

void TaskManager::init() {
    if (!procs.empty()) return;

    procs.push_back({"System",            4,   2.4f,  128.0f});
    procs.push_back({"csopesy_shell.exe", 128, 8.7f,  86.5f});
    procs.push_back({"taskbar.exe",       256, 3.2f,  42.8f});
    procs.push_back({"desktop.exe",       312, 5.9f,  96.4f});
    procs.push_back({"explorer.exe",      420, 7.1f,  154.2f});
    procs.push_back({"sysinfo.exe",       512, 1.8f,  38.6f});
    procs.push_back({"files.exe",         608, 4.5f,  72.3f});
    procs.push_back({"imgui_render.exe",  724, 11.6f, 118.7f});
    procs.push_back({"glfw.exe",          816, 2.1f,  31.9f});
    procs.push_back({"audio_service.exe", 904, 0.9f,  24.5f});
}

void TaskManager::updateFakeValues() {
    if (procs.empty()) init();

    double now = glfwGetTime();
    if (now - lastUpdate < 1.0) return;
    lastUpdate = now;

    for (auto& process : procs) {
        float cpuChange = static_cast<float>((std::rand() % 1001 - 500) / 100.0f);
        process.cpu += cpuChange;
        process.cpu = std::max(0.0f, std::min(100.0f, process.cpu));

        float memChange = static_cast<float>((std::rand() % 1001 - 500) / 100.0f);
        process.memMB += memChange;
        process.memMB = std::max(8.0f, process.memMB);
    }
}

void TaskManager::render(OSState& os) {
    if (!os.showTaskMgr) return;
    if (procs.empty()) init();

    ImGui::SetNextWindowSize(ImVec2(650, 430), ImGuiCond_FirstUseEver);
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
    float totalCpu = 0.0f;
    float totalMemory = 0.0f;

    for (const auto& process : procs) {
        totalCpu += process.cpu;
        totalMemory += process.memMB;
    }

    float cpuPercent = std::min(totalCpu, 100.0f);

    ImGui::Text("Task Manager");
    ImGui::SameLine();
    ImGui::TextDisabled("| %zu processes", procs.size());

    ImGui::Spacing();

    ImGui::Text("CPU");
    ImGui::SameLine(75.0f);
    ImGui::ProgressBar(cpuPercent / 100.0f, ImVec2(210, 18), "");
    ImGui::SameLine();
    ImGui::Text("%.1f%%", cpuPercent);

    ImGui::Text("Memory");
    ImGui::SameLine(75.0f);
    ImGui::Text("%.1f MB used", totalMemory);

    ImGui::Spacing();
}

void TaskManager::drawProcessTable() {
    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("procs", 4, flags, ImVec2(0, 285))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Process", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Memory (MB)", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableHeadersRow();

        for (const auto& process : procs) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(process.name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", process.pid);

            ImGui::TableSetColumnIndex(2);
            if (process.cpu > 50.0f)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.35f, 0.30f, 1.0f));
            ImGui::Text("%.1f%%", process.cpu);
            if (process.cpu > 50.0f)
                ImGui::PopStyleColor();

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.1f", process.memMB);
        }

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::TextDisabled("Dummy process values update approximately every second.");
}
