#pragma once

#include <fstream>

namespace mfrt
{
    class InfoBlock
    {
    public:
        InfoBlock() = default;
        InfoBlock(int argc, char *argv[], bool doLog = false);

        std::ifstream &GetFrtIn();
        std::ofstream &GetCOut();
        std::string GetOptions() const;
        std::string GetTempC() const;
        std::string GetTempFrt() const;
        std::string GetInputFrt() const;
        std::string GetExecName() const;

    private:
        std::ifstream frtIn;
        std::ofstream cOut;
        std::string tempC, tempFrt, inputFrt;
        std::string options;

    private:
        void SetOptions(int argc, char *argv[]);
    };
}
