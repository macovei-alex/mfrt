#include <iostream>

#include "..\include\InfoBlock.h"
#include "..\include\Logger.h"
#include "..\include\funcpp.h"

int main(int argc, char *argv[])
{
    Logger logger{std::cout};
    try
    {
        mfrt::InfoBlock info{argc, argv};
        mfrt::WriteCpp(info, logger);
        mfrt::CompileCommand(info);
        if (info.GetOptions().find("-r") != std::string::npos)
            mfrt::ExecuteProgram(info);
    }
    catch (const std::exception &e)
    {
        logger.Log(Logger::LogLevel::Error, "Could not initialize the InfoBlock object\n");
        return 1;
    }

    return 0;
}