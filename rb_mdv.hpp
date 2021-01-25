
#pragma once

#include <string>
#include <vector>

typedef unsigned long U32;
typedef unsigned short U16;
typedef unsigned char U8;

class RBMdv {
public:
    RBMdv();

    void Load(std::string name, std::string path);
    void Unload();
    std::vector<std::string> List();
    void Add(std::string filename);
    void ExportAll();
    void Delete(std::string filename);

    bool IsLoaded() { return m_loaded; }
    int NumberOfFiles();

    std::string GetMdvName() { return m_name; }
    std::string GetMdvPath() { return m_path; }
    std::string GetFilename(int index, bool includePath);
    void SetTemporaryPath(std::string path);
    
private:
    void AddAll();
    void LoadFiles();

// Internal MDV access files, refactored from qlt
private:
    void PutLong(U8 *p, U32 v);
    void PutWord(U8 *p, U16 v);
    U32 GetLong(U8 *p);
    U16 GetWord(U8 *p);
    int GetXtcc(FILE *f, U32 *d);
    void OutputFile(int fnum);
    int PrintMap(int create);
    void WriteDirLen(void);
    void PutDir(void);
    void InitMdv(void);
    void PutSect(int sectlen);
    void WriteMdv(U8 *buffer, FILE *outfile);
    int GenHeader(char *ifname, U8 *head, int dz, U32 datasize);
    int File2Mdv(char *fname, char *ofname);
    int Mdv2File(char *fname, int create);

private:
    bool m_loaded;
    std::string m_temporaryPath;
    std::string m_path;
    std::string m_name;
    std::vector<std::string> m_files;
};

