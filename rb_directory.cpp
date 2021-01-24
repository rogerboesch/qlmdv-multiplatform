
#include "rb_directory.hpp"
#include <glob.h>

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

