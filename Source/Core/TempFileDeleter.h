﻿#ifndef IU_CORE_TEMPFILEDELETER_H
#define IU_CORE_TEMPFILEDELETER_H

#pragma once
#include "Core/Utils/CoreTypes.h"
#include <string>
#include <vector>

class TempFileDeleter
{
public:
    TempFileDeleter();
    ~TempFileDeleter();
    void addFile(const std::string& fileName);
    bool cleanup();

protected:
    std::vector<std::string> m_files;
private:
    DISALLOW_COPY_AND_ASSIGN(TempFileDeleter);
};

#endif
