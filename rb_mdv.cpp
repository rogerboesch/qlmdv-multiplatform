
#include "rb_mdv.hpp"

extern "C" {
    int fil2mdv(char*, char*);
    int mdv2fil(char*, int);

    extern char filenames[256][37];
    extern char temppath[256];
}

RBMdv::RBMdv() {
    m_name = "";
    m_path = "";
    m_loaded = false;
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

std::string RBMdv::GetFilename(int index, bool includePath) {
    if (index >= 0 && index < m_files.size()) {
        if (!includePath) {
            return m_files[index];
        }
        
        std::string path = m_temporaryPath + "/" + m_files[index];
        return path;
    }
    
    return "";
}

void RBMdv::Add(std::string filename) {
    m_files.emplace_back(filename);
    LoadFiles();
}

void RBMdv::ExportAll() {
    std::string path = m_path + "/" + m_name;
    
    printf("Export all files to %s\n", m_temporaryPath.c_str());
    
    int result = mdv2fil((char *)path.c_str(), 1);
}

void RBMdv::Delete(std::string filename) {
}

void RBMdv::AddAll() {
}

void RBMdv::LoadFiles() {
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

void RBMdv::SetTemporaryPath(std::string path) {
    m_temporaryPath = path;
    strcpy(temppath, path.c_str());
}
