#include "AppFiles.h"
#include "Taskbar.h"
#include "imgui.h"
#include <algorithm>
#include <string>

namespace {
struct FileEntry {
    int parent;
    const char* name;
    const char* type;
    const char* size;
    const char* modified;
    bool folder;
    const char* preview;
};

// folder IDs are array indexes, so parents and sidebar shortcuts stay stable
constexpr FileEntry kEntries[] = {
    {-1, "Local Disk (C:)", "Folder", "--", "2026-09-18", true, nullptr},
    {0, "Documents", "Folder", "--", "2026-09-18", true, nullptr},
    {0, "Downloads", "Folder", "--", "2026-09-18", true, nullptr},
    {0, "Pictures", "Folder", "--", "2026-09-18", true, nullptr},
    {0, "Projects", "Folder", "--", "2026-09-18", true, nullptr},
    {1, "CSOPESY 01 - Introduction.pdf", "PDF", "1.2 MB", "2026-09-18", false, nullptr},
    {1, "CSOPESY 02 - Operating System Structure.pdf", "PDF", "1.8 MB", "2026-09-18", false, nullptr},
    {1, "CSOPESY 03 - IO and Display.pdf", "PDF", "1.5 MB", "2026-09-18", false, nullptr},
    {1, "CSOPESY 04 - Processes.pdf", "PDF", "2.1 MB", "2026-09-18", false, nullptr},
    {1, "CSOPESY 05 - Threads.pdf", "PDF", "1.7 MB", "2026-09-18", false, nullptr},
    {1, "CSOPESY 06 - CPU Scheduling.pdf", "PDF", "2.4 MB", "2026-09-18", false, nullptr},
    {1, "notes.txt", "Text", "1 KB", "2026-09-18", false,
        "CSOPESY study notes\n\nRemember: a process owns resources, while threads share a process's address space.\n\nNext on the reading list: CPU Scheduling. Good luck on the quiz!"},
    {2, "csopesy_setup.exe", "Application", "8.4 MB", "2026-09-18", false, nullptr},
    {2, "Archive", "Folder", "--", "2026-09-18", true, nullptr},
    {3, "wallpaper.jpg", "JPEG image", "2.3 MB", "2026-09-18", false, nullptr},
    {3, "group_photo.png", "PNG image", "940 KB", "2026-09-18", false, nullptr},
    {4, "CSOPESY-MO4", "Folder", "--", "2026-09-18", true, nullptr},
    {16, "README.txt", "Text", "2 KB", "2026-09-18", false,
        "CSOPESY OS v1.0\n\nA desktop-style OS mock-up built with C++17, GLFW, OpenGL, and Dear ImGui.\n\nExplore the desktop, open Task Manager, and use PWR when you're ready to shut down."},
    {16, "main.cpp", "C++ source", "8 KB", "2026-09-18", false,
        "// A tiny sample from our imaginary project\nint main() {\n    // boot -> desktop -> app windows -> taskbar\n    return 0;\n}"},
};
constexpr int kEntryCount = sizeof(kEntries) / sizeof(kEntries[0]);

std::string folderPath(int folder) {
    if (folder == 0) return "This PC > Local Disk (C:)";
    return folderPath(kEntries[folder].parent) + " > " + kEntries[folder].name;
}

void keepWindowInDesktop() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    const ImVec2 pos = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetWindowSize();
    ImGui::SetWindowPos(ImVec2(
        std::max(vp->Pos.x + 16.f, std::min(pos.x, vp->Pos.x + vp->Size.x - size.x - 16.f)),
        std::max(vp->Pos.y + 16.f, std::min(pos.y, vp->Pos.y + vp->Size.y - Taskbar::HEIGHT - size.y - 16.f))));
}
}

void AppFiles::render(OSState& os) {
    if (!os.showFiles) return;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    const ImVec2 available(std::max(1.f, vp->Size.x - 32.f),
                           std::max(1.f, vp->Size.y - Taskbar::HEIGHT - 32.f));
    const ImVec2 size(std::min(900.f, available.x), std::min(460.f, available.y));
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x + 16.f, vp->Pos.y + 16.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(std::min(480.f, available.x),
                                               std::min(260.f, available.y)), available);
    const bool expanded = ImGui::Begin("File Explorer", &os.showFiles);
    keepWindowInDesktop();
    if (expanded) {
        drawToolbar();
        ImGui::Separator();

        const float footerHeight = ImGui::GetTextLineHeightWithSpacing() * 2.f + 12.f;
        bool openAfterContent = false;
        if (ImGui::BeginChild("browser", ImVec2(0, -footerHeight))) {
            if (ImGui::BeginChild("locations", ImVec2(140.f, 0), ImGuiChildFlags_Borders)) {
                drawSidebar();
            }
            ImGui::EndChild();
            ImGui::SameLine();
            if (ImGui::BeginChild("contents", ImVec2(0, 0))) {
                openAfterContent = drawFileTable();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
        if (openAfterContent) openSelected();
        ImGui::Separator();
        drawStatusBar();
        drawPreview();
    }
    ImGui::End();
}

void AppFiles::navigateTo(int folder) {
    if (folder < 0 || folder >= kEntryCount || !kEntries[folder].folder) return;
    currentFolder = folder;
    selectedEntry = -1;
    resetFileScroll = true;
}

void AppFiles::openSelected() {
    if (selectedEntry < 0 || selectedEntry >= kEntryCount) return;
    if (kEntries[selectedEntry].folder) {
        navigateTo(selectedEntry);
    } else {
        previewEntry = selectedEntry;
        ImGui::OpenPopup("File Preview");
    }
}

void AppFiles::drawToolbar() {
    ImGui::BeginDisabled(currentFolder == 0);
    if (ImGui::Button("Up")) navigateTo(kEntries[currentFolder].parent);
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Home")) navigateTo(0);
    ImGui::SameLine();
    ImGui::BeginDisabled(selectedEntry < 0);
    if (ImGui::Button("Open")) openSelected();
    ImGui::EndDisabled();
    ImGui::TextWrapped("%s", folderPath(currentFolder).c_str());
}

void AppFiles::drawSidebar() {
    ImGui::TextDisabled("LOCATIONS");
    ImGui::Spacing();
    if (ImGui::Selectable("This PC", currentFolder == 0)) navigateTo(0);
    ImGui::Separator();
    for (int i = 1; i <= 4; ++i) {
        // highlight the shortcut even when browsing one of its subfolders
        int ancestor = currentFolder;
        while (ancestor > 0 && ancestor != i) ancestor = kEntries[ancestor].parent;
        if (ImGui::Selectable(kEntries[i].name, ancestor == i)) navigateTo(i);
    }
}

bool AppFiles::drawFileTable() {
    const ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;
    bool openAfterTable = false;
    if (ImGui::BeginTable("files", 4, flags, ImVec2(0, 0), 720.f)) {
        if (resetFileScroll) {
            ImGui::SetScrollY(0.f);
            ImGui::SetScrollX(0.f);
            resetFileScroll = false;
        }
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 85.f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 65.f);
        ImGui::TableSetupColumn("Date Modified", ImGuiTableColumnFlags_WidthFixed, 110.f);
        ImGui::TableHeadersRow();

        int itemCount = 0;
        for (int i = 1; i < kEntryCount; ++i) {
            const FileEntry& entry = kEntries[i];
            if (entry.parent != currentFolder) continue;
            ++itemCount;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(i);
            if (ImGui::Selectable(entry.name, selectedEntry == i,
                    ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
                selectedEntry = i;
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) openAfterTable = true;
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", entry.name);
            ImGui::PopID();
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(entry.type);
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(entry.size);
            ImGui::TableSetColumnIndex(3);
            ImGui::TextUnformatted(entry.modified);
        }
        if (itemCount == 0) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("This folder is empty.");
        }
        ImGui::EndTable();
    }
    // open from File Explorer after its child panels end, so popup IDs match
    return openAfterTable;
}

void AppFiles::drawStatusBar() {
    int itemCount = 0;
    for (int i = 1; i < kEntryCount; ++i) {
        if (kEntries[i].parent == currentFolder) ++itemCount;
    }
    ImGui::Text("%d items | Simulated files", itemCount);
    if (selectedEntry >= 0) {
        ImGui::TextWrapped("Selected: %s", kEntries[selectedEntry].name);
    } else {
        ImGui::TextDisabled("Double-click an item or select it and press Open.");
    }
}

void AppFiles::drawPreview() {
    if (previewEntry < 0) return;
    ImGuiViewport* vp = ImGui::GetMainViewport();
    const ImVec2 available(std::max(1.f, vp->Size.x - 32.f),
                           std::max(1.f, vp->Size.y - Taskbar::HEIGHT - 32.f));
    ImGui::SetNextWindowSize(ImVec2(std::min(580.f, available.x),
                                    std::min(340.f, available.y)), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x + vp->Size.x * 0.5f,
                                  vp->Pos.y + (vp->Size.y - Taskbar::HEIGHT) * 0.5f),
                            ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(std::min(320.f, available.x),
                                               std::min(220.f, available.y)), available);
    bool showPreview = true;
    if (ImGui::BeginPopupModal("File Preview", &showPreview)) {
        keepWindowInDesktop();
        const FileEntry& entry = kEntries[previewEntry];
        const float footerHeight = ImGui::GetFrameHeightWithSpacing() + 8.f;
        if (ImGui::BeginChild("previewContent", ImVec2(0, -footerHeight))) {
            ImGui::TextWrapped("%s", entry.name);
            ImGui::TextDisabled("%s | %s | %s", entry.type, entry.size, entry.modified);
            ImGui::Separator();
            if (entry.preview) {
                ImGui::TextWrapped("%s", entry.preview);
            } else {
                ImGui::TextWrapped("This is a simulated file. Its details are available here; a document or image viewer is not installed.");
            }
        }
        ImGui::EndChild();
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            showPreview = false;
        }
        ImGui::EndPopup();
    }
    if (!showPreview || !ImGui::IsPopupOpen("File Preview")) previewEntry = -1;
}
