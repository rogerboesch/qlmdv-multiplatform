
#pragma once

#include <string>
#include <vector>

class RBMdv {
public:
    RBMdv();

    void Load(std::string name, std::string path);
    void Unload();
    std::vector<std::string> List();
    void Add(std::string filename);
    void Export(std::string filename);
    void Delete(std::string filename);

    bool IsLoaded() { return m_loaded; }
    int NumberOfFiles();

    std::string GetName() { return m_name; }
    std::string GetPath() { return m_path; }

private:
    void AddAll();
    void LoadFiles();
    
private:
    bool m_loaded;
    std::string m_path;
    std::string m_name;
    std::vector<std::string> m_files;
};

