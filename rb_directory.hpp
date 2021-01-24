
#pragma once

#include <string>
#include <vector>

class RBDirectory {
public:
    RBDirectory();

    void AddPath(std::string path);
    void RemovePath();
    
    std::vector<std::string> List() { return m_files; }

private:
    void Read();
    std::string BuildPath();

private:
    std::vector<std::string> m_path;
    std::vector<std::string> m_files;
};

