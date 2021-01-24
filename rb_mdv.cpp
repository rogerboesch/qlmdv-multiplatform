
#include "rb_mdv.hpp"

extern "C" {
    #include "qlayt.h"
}

RBMdv::RBMdv() {
    m_name = "";
    m_path = "";
}

void RBMdv::Load(std::string name, std::string path) {
    m_name = name;
    m_path = path;
    
    LoadFiles();
}

void RBMdv::Unload() {
    m_name = "";
    m_path = "";
    m_files.clear();
    m_loaded = false;
}

std::vector<std::string> RBMdv::List() {
    return m_files;
}

int RBMdv::NumberOfFiles() {
    return m_files.size();
}

void RBMdv::Add(std::string filename) {
    m_files.emplace_back(filename);
    LoadFiles();
}

void RBMdv::Export(std::string filename) {
    return false;
}

void RBMdv::Delete(std::string filename) {
    return false;
}

void RBMdv::AddAll() {
    return false;
}

void RBMdv::LoadFiles() {
    resetglobals();
    
    std::string path = m_path + "/" + m_name;
    int result = mdv2fil((char *)path.c_str(), 0);

    m_files.clear();
    
    for (int i = 0; i < result; ++i) {
        if (strlen(filenames[i]) > 0) {
            m_files.emplace_back(filenames[i]);
        }
    }
    
    m_loaded = true;
}
