
#pragma once

#include "rb_directory.hpp"
#include "rb_mdv.hpp"

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
    
    int m_selectedDir;
    int m_selectedFile;
    std::string m_filename;
};
