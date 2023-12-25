#include "..\include\InfoBlock.h"

#include <windows.h>
#include <iostream>

mfrt::InfoBlock::InfoBlock(int argc, char* argv[], bool doLog)
{
    this->SetOptions(argc, argv);

    char* execPath = new char[MAX_PATH];
    GetModuleFileName(NULL, execPath, sizeof(execPath));
    std::string tempDirPath{ std::move(execPath) };
    if (tempDirPath.empty())
        throw std::exception{ "Could not get the module file name" };

    if (doLog)
        std::cout << "EXEC_PATH: " << tempDirPath << std::endl;

    // Remove the executable name to get the directory
    size_t pos;
    if ((pos = tempDirPath.find('\\')) != std::string::npos)
        tempDirPath.erase(pos, tempDirPath.length() - pos);
    else
        throw std::exception{ "Character \'\\\' not found" };
    if ((pos = tempDirPath.find('\\')) != std::string::npos)
        tempDirPath.erase(pos, tempDirPath.length() - pos);
    else
        throw std::exception{ "Character \'\\\' not found; Path tpp short" };

    tempDirPath += "\\temp";
    this->tempFrt = tempDirPath + "\\src.frt";
    this->tempC = tempDirPath + "\\src.c";

    if (doLog)
    {
        std::cout << "TEMP_DIR: " << execPath << '\n';
        std::cout << "TEMP_FRT_PATH: " << this->tempFrt << '\n';
        std::cout << "TEMP_C_PATH: " << this->tempC << '\n';
    }

    if (system(std::string{ "copy " + this->inputFrt + " " + this->tempFrt }.c_str()))
        throw std::exception{ "Copy command unsuccessful" };

    this->frtIn = std::ifstream(this->tempFrt);
    if (!this->frtIn.good())
        throw std::exception{ "Could not open the temporary .frt file" };
    this->cOut = std::ofstream(this->tempC);
    if (!this->cOut.good())
        throw std::exception{ "Could not open the temporary .cpp file" };
}

std::ifstream& mfrt::InfoBlock::GetFrtIn()
{
    return this->frtIn;
}

std::ofstream& mfrt::InfoBlock::GetCOut()
{
    return this->cOut;
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

std::string mfrt::InfoBlock::GetExecName() const
{
    std::string execName{ this->inputFrt.begin(), this->inputFrt.end() };
    execName += ".exe";
    return execName;
}

void mfrt::InfoBlock::SetOptions(int argc, char* argv[])
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