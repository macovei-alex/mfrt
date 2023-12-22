#include "InfoBlock.h"

#include <windows.h>
#include <iostream>

mfrt::InfoBlock::InfoBlock(int argc, char *argv[], bool doLog)
{
    this->SetOptions(argc, argv);

    char *execPath = new char[MAX_PATH];
    GetModuleFileName(NULL, execPath, sizeof(execPath));
    std::string tempDirPath{std::move(execPath)};

    if (doLog)
        std::cout << "EXEC_PATH: " << tempDirPath << std::endl;

    // Remove the executable name to get the directory
    size_t pos;
    if ((pos = tempDirPath.find('\\')) != std::string::npos)
        tempDirPath.erase(pos, tempDirPath.length() - pos);
    if ((pos = tempDirPath.find('\\')) != std::string::npos)
        tempDirPath.erase(pos, tempDirPath.length() - pos);

    tempDirPath += "\\temp";
    this->tempFrt = tempDirPath + "\\src.frt";
    this->tempC = tempDirPath + "\\src.c";

    if (doLog)
    {
        std::cout << "TEMP_DIR: " << execPath << '\n';
        std::cout << "TEMP_FRT_PATH: " << this->tempFrt << '\n';
        std::cout << "TEMP_C_PATH: " << this->tempC << '\n';
    }

    system(std::string{"copy " + this->inputFrt + " " + this->tempFrt}.c_str());

    this->frtIn = std::ifstream(this->tempFrt);
    this->cIn = std::ifstream(this->tempC);
}

std::ifstream &mfrt::InfoBlock::GetFrtIn()
{
    return this->frtIn;
}

std::ifstream &mfrt::InfoBlock::GetCIn()
{
    return this->cIn;
}

std::string mfrt::InfoBlock::GetOptions() const
{
    return this->options;
}

std::string mfrt::InfoBlock::GetTempC() const
{
    return this->tempC;
}

std::string mfrt::InfoBlock::GetTempFrt() const
{
    return this->tempFrt;
}

std::string mfrt::InfoBlock::GetInputFrt() const
{
    return this->inputFrt;
}

void mfrt::InfoBlock::SetOptions(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            this->options += argv[i];
            this->options += ' ';
        }
    }
}