
#include "qlmdv_app.hpp"
#include <filesystem>
#include <fstream>
#include <streambuf>

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
    m_selectedDir = 0;
    m_selectedFile = 0;
    m_currentDir = -1;
    m_currentFile = -1;
    m_filename = "";
    
    std::string path = std::__fs::filesystem::temp_directory_path();
    m_mdv.SetTemporaryPath(path);
    
    auto lang = TextEditor::LanguageDefinition::SuperBasic();
    m_editor.SetLanguageDefinition(lang);
}

void QLMdvApp::MapDirectory() {
    std::vector<std::string> values = m_directory.List();
    ImGui::PushItemWidth(200);
    rbListBox("directory", &m_selectedDir, values);
    ImGui::PopItemWidth();

    if (m_selectedDir != m_currentDir) {
        m_currentDir = m_selectedDir;
        
        if (m_directory.IsFile(m_selectedDir)) {
            std::string name = m_directory.GetName(m_selectedDir);
            std::string path = m_directory.GetPath();
            m_mdv.Load(name, path);
            
            m_filename = "";
            m_currentFile = 0;
            m_selectedFile = -1;
        }
    }
    
    if (ImGui::IsMouseDoubleClicked(0)) {
        m_directory.JumpTo(m_selectedDir);
        m_mdv.Unload();
        
        m_selectedFile = 0;
        m_currentFile = -1;
        m_filename = "";
    }
}

void QLMdvApp::MapMdv() {
    if (m_mdv.IsLoaded()) {
        if (m_mdv.NumberOfFiles() > 0) {
            std::vector<std::string> values = m_mdv.List();
            ImGui::PushItemWidth(200);
            rbListBox("mdv", &m_selectedFile, values);
            ImGui::PopItemWidth();

            if (m_selectedFile != m_currentFile) {
                m_currentFile = m_selectedFile;
                
                m_mdv.ExportAll();
                
                m_filename = m_mdv.GetFilename(m_currentFile, true);
                
                // Load file
                std::ifstream t(m_filename);
                
                if (t.good()) {
                    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                    m_editor.SetText(str);
                }
                else {
                    m_filename = "";
                }
            }
        }
        else {
            ImGui::TextWrapped("MDV image is empty");
        }
    }
    else {
        ImGui::TextWrapped("No MDV image selected");
    }
}

void QLMdvApp::MapFilePreview() {
    if (m_filename.size() > 0) {
        m_editor.Render(m_filename.c_str());
    }
    else {
        ImGui::TextWrapped("No file selected");
    }
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
    
    if (m_mdv.IsLoaded()) {
        ImGui::SameLine(0, 10);
        ImGui::Button("Add File", (ImVec2){0,0});
    }
    
    if (m_filename.size() > 0) {
        ImGui::SameLine(0, 10);
        ImGui::Button("Remove File", (ImVec2){0,0});
    }
    
    if (m_filename.size() > 0) {
        ImGui::SameLine(0, 10);
        ImGui::Button("Export File", (ImVec2){0,0});
    }

    ImGui::EndGroup();
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(panel_margin, panel_top), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(panel_width, panel_height), ImGuiCond_Once);
    ImGui::Begin("Directories", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    MapDirectory();
    ImGui::End();

    std::string mdvTitle = m_mdv.IsLoaded() ? m_mdv.GetMdvName() : "MDV Image";
    ImGui::SetNextWindowPos(ImVec2(2*panel_margin+panel_width, panel_top), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(panel_width, panel_height), ImGuiCond_Once);
    ImGui::Begin(mdvTitle.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    MapMdv();
    ImGui::End();

    std::string name = "Editor";
    if (m_filename.size() > 0) {
        name = name + ": " + m_mdv.GetFilename(m_currentFile, false);
    }
    
    ImGui::SetNextWindowPos(ImVec2(3*panel_margin+2*panel_width, panel_top), ImGuiCond_Once, (ImVec2){0,0});
    ImGui::SetNextWindowSize(ImVec2(last_panel_width, panel_height), ImGuiCond_Once);
    ImGui::Begin(name.c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    MapFilePreview();
    ImGui::End();
}

extern "C" {
    void rbFrame(const float width, const float height) {
        app.Frame(width, height);
    }

    const char* rbGetWindowTitle() {
        return "QLmdv 0.9.0";
    }

    ImVec2 rbGetWindowSize() {
        return ImVec2(800, 600);
    }
}
