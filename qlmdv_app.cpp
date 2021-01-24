
#include "qlmdv_app.hpp"

#include "./imgui/imgui.h"
#ifdef CIMGUI_FREETYPE
#include "./imgui/misc/freetype/imgui_freetype.h"
#endif
#include "./imgui/imgui_internal.h"
#include "cimgui.h"

static auto vector_getter = [](void* vec, int idx, const char** out_text) {
    auto& vector = *static_cast<std::vector<std::string>*>(vec);
    if (idx < 0 || idx >= static_cast<int>(vector.size())) {
        return false;
    }
    
    *out_text = vector.at(idx).c_str();
    return true;
};

bool rbCombo(const char* label, int* currIndex, std::vector<std::string>& values) {
    if (values.empty()) {
        return false;
    }
    
    return ImGui::Combo(label, currIndex, vector_getter, static_cast<void*>(&values), values.size());
}

bool rbListBox(const char* label, int* currIndex, std::vector<std::string>& values) {
    if (values.empty()) {
        return false;
    }
    
    return ImGui::ListBox(label, currIndex, vector_getter, static_cast<void*>(&values), values.size(), 30);
}

QLMdvApp app;

QLMdvApp::QLMdvApp() {
}

void QLMdvApp::MapDirectory() {
    int selectedLine;
    std::vector<std::string> values = m_directory.List();
    igPushItemWidth(200);
    rbListBox("", &selectedLine, values);
    igPopItemWidth();
}

void QLMdvApp::Frame(const float width, const float height) {
    const int toolbar_height = 30;
    const int panel_margin = 5;
    const int panel_top = toolbar_height+2*panel_margin;
    const int panel_width = 200;
    const int panel_height = height-toolbar_height-3*panel_margin;
    const int last_panel_width = width-4*panel_margin-2*panel_width;

    ImGui::SetNextWindowPos(ImVec2(0, panel_margin), ImGuiCond_Once, (ImVec2){0, 0});
    ImGui::SetNextWindowSize(ImVec2(width, toolbar_height), ImGuiCond_Once);
    ImGui::Begin("Toolbar", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::BeginGroup();
    ImGui::Button("New MDV", (ImVec2){0,0});
    ImGui::SameLine(0, 10);
    ImGui::Button("Export File", (ImVec2){0,0});
    ImGui::SameLine(0, 10);
    ImGui::Button("Add File", (ImVec2){0,0});
    ImGui::EndGroup();
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(panel_margin, panel_top), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(panel_width, panel_height), ImGuiCond_Once);
    ImGui::Begin("Directories", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    MapDirectory();
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(2*panel_margin+panel_width, panel_top), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(panel_width, panel_height), ImGuiCond_Once);
    ImGui::Begin("MDV image", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(3*panel_margin+2*panel_width, panel_top), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(last_panel_width, panel_height), ImGuiCond_Once);
    ImGui::Begin("Preview", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    ImGui::End();
}

extern "C" {
    void rbFrame(const float width, const float height) {
        app.Frame(width, height);
    }
}
