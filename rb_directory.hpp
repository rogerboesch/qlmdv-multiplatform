
#pragma once

#include <string>
#include <vector>

class RBDirectory {
public:
    RBDirectory();

    bool JumpTo(int index);
    bool IsFile(int index);
    bool IsDirectory(int index);

    std::string GetName(int index);
    std::string GetPath();

    std::vector<std::string> List() { return m_files; }

private:
    bool AddPath(std::string path);
    void RemovePath();
    void Read();
    std::string BuildPath();

private:
    std::vector<std::string> m_path;
    std::vector<std::string> m_files;
};

