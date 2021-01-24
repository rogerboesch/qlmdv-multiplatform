
#pragma once

#include "rb_directory.hpp"
#include "rb_mdv.hpp"
#include "texteditor/TextEditor.h"

class QLMdvApp {
public:
    QLMdvApp();
    
public:
    void Frame(const float width, const float height);

private:
    void MapDirectory();
    void MapMdv();
    void MapFilePreview();

private:
    RBDirectory m_directory;
    RBMdv m_mdv;
    TextEditor m_editor;

    int m_selectedDir;
    int m_selectedFile;
    int m_currentDir;
    std::string m_filename;
};
