
#pragma once

#include "rb_directory.hpp"

class QLMdvApp {
public:
    QLMdvApp();
    
public:
    void Frame(const float width, const float height);

private:
    void MapDirectory();

private:
    RBDirectory m_directory;
};

