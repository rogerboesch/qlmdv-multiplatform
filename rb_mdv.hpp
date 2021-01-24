
#pragma once

#include <string>
#include <vector>

class RBMdv {
public:
    RBMdv();

    void Load(std::string name, std::string path);
    std::vector<std::string> List();
    void Add(std::string filename);
    void Export(std::string filename);
    void Delete(std::string filename);

    int NumberOfFiles();
    
private:
    void AddAll();
    void LoadFiles();
    
private:
    std::string m_path;
    std::string m_name;
    std::vector<std::string> m_files;
};

