#pragma once

#include "InfoBlock.h"
#include "Logger.h"

namespace mfrt
{
    size_t GetInt(InfoBlock &info, Logger &logger);
    void Print(InfoBlock &info, const std::string &message, size_t tabCount);
    int WriteCpp(InfoBlock &info, Logger &logger);
    int CompileCommand(InfoBlock &info);
    int ExecuteProgram(InfoBlock &info);
}
