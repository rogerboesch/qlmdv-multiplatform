/*
    Based on Qlay Tools by Jan Venema (C) 1998
    C++ Code by Roger Boesch
    (Will replace qlt_mdv.c in future to use C++ 14+)
*/

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

// Internal MDV access files, refactored from qlt (In progress)
void RBMdv::PutLong(U8 *p, U32 v) {}
void RBMdv::PutWord(U8 *p, U16 v) {}
U32 RBMdv::GetLong(U8 *p) {}
U16 RBMdv::GetWord(U8 *p) {}
int RBMdv::GetXtcc(FILE *f, U32 *d) {}
void RBMdv::OutputFile(int fnum) {}
int RBMdv::PrintMap(int create) {}
void RBMdv::WriteDirLen(void) {}
void RBMdv::PutDir(void) {}
void RBMdv::InitMdv(void) {}
void RBMdv::PutSect(int sectlen) {}
void RBMdv::WriteMdv(U8 *buffer, FILE *outfile) {}
int RBMdv::GenHeader(char *ifname, U8 *head, int dz, U32 datasize) {}
int RBMdv::File2Mdv(char *fname, char *ofname) {}
int RBMdv::Mdv2File(char *fname, int create) {}
