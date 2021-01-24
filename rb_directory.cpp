
#include "rb_directory.hpp"
#include <glob.h>

#include "./imgui/imgui.h"
#ifdef CIMGUI_FREETYPE
#include "./imgui/misc/freetype/imgui_freetype.h"
#endif
#include "./imgui/imgui_internal.h"
#include "cimgui.h"

static RBDirectory directory;

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

extern "C" {
    void rbigMapListDirectory(void) {
        int selectedLine;
        std::vector<std::string> values = directory.List();
        igPushItemWidth(200);
        rbListBox("", &selectedLine, values);
        igPopItemWidth();
    }
}

RBDirectory::RBDirectory() {
    Read();
}

void RBDirectory::AddPath(std::string path) {
    m_path.emplace_back(path);
    Read();
}

void RBDirectory::RemovePath() {
    if (m_path.size() > 0) {
        m_path.pop_back();
        Read();
    }
}

std::string RBDirectory::BuildPath() {
    if (m_files.size() == 0) {
        return "";
    }
    
    std::string path;
    
    for (auto entry : m_path) {
        path = path + "/" + entry;
    }
    
    return path;
}

void RBDirectory::Read() {
    std::string path = BuildPath() + "/*";
    
    m_files.clear();

    glob_t glob_result;
    glob(path.c_str(), GLOB_TILDE, NULL, &glob_result);
    
    for (unsigned int i = 0; i<glob_result.gl_pathc; ++i) {
        m_files.emplace_back(glob_result.gl_pathv[i]);
    }
}

