
#include "rb_directory.hpp"
#include <glob.h>
#include <sys/stat.h>

RBDirectory::RBDirectory() {
    Read();
}

std::string RBDirectory::GetName(int index) {
    if (index >= 0 && index < m_files.size()) {
        std::string path = m_files[index];
        return path;
    }
    
    return "";
}

std::string RBDirectory::GetPath() {
    return BuildPath();
}

bool RBDirectory::JumpTo(int index) {
    if (index >= 0 && index < m_files.size()) {
        std::string path = m_files[index];
        if (path == "..") {
            RemovePath();
            return true;
        }
        
        return AddPath(path);
    }
    
    return false;
}

bool RBDirectory::IsFile(int index) {
    if (index >= 0 && index < m_files.size()) {
        std::string name = m_files[index];
        std::string new_path = BuildPath() + "/" + name;

        // Test path
        struct stat s;
        if (stat(new_path.c_str(), &s) == 0) {
            if (s.st_mode & S_IFDIR) {
                // it's a directory
                return false;
            }
            else if (s.st_mode & S_IFREG) {
                // it's a file
                return true;
            }
        }
    }
    
    return false;
}

bool RBDirectory::IsDirectory(int index) {
    if (index >= 0 && index < m_files.size()) {
        std::string name = m_files[index];
        std::string new_path = BuildPath() + "/" + name;

        // Test path
        struct stat s;
        if (stat(new_path.c_str(), &s) == 0) {
            if (s.st_mode & S_IFDIR) {
                // it's a directory
                return true;
            }
            else if (s.st_mode & S_IFREG) {
                // it's a file
                return false;
            }
        }
    }
    
    return false;
}

bool RBDirectory::AddPath(std::string path) {
    m_path.push_back(path);
    std::string new_path = BuildPath();

    // Test path
    struct stat s;
    if (stat(new_path.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            // it's a directory
            Read();
            return true;
        }
        else if (s.st_mode & S_IFREG) {
            // it's a file
            m_path.pop_back();
            return false;
        }
    }
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
    
    if (m_path.size() > 0) {
        m_files.push_back("..");
    }
    
    glob_t glob_result;
    glob(path.c_str(), GLOB_TILDE, NULL, &glob_result);
    
    for (unsigned int i = 0; i< glob_result.gl_pathc; ++i) {
        std::string path = glob_result.gl_pathv[i];
        std::string last_part = path.substr(path.find_last_of("/\\") + 1);
        m_files.push_back(last_part);
    }
}

