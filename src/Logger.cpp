#include "..\include\Logger.h"

#include <format>

Logger::Logger(std::ostream& logStream) : logStream{ logStream }, currentPosition{ 0, 0 }
{
    /* empty */
}

Logger::Logger(std::ostream&& logStream) : logStream{ logStream }, currentPosition{ 0, 0 }
{
    /* empty */
}

void Logger::Log(LogLevel level, const std::string& message, bool logPosition)
{
    std::string logLevelStr{};
    if (level == LogLevel::Info)
        logLevelStr = "Info";
    else if (level == LogLevel::Warning)
        logLevelStr = "Warning";
    else if (level == LogLevel::Error)
        logLevelStr = "Error";

    if (logPosition)
        logStream << std::format("[{}] {}:{}: {}\n", logLevelStr, this->currentPosition.first, this->currentPosition.second, message);
    else
        logStream << std::format("[{}] {}\n", logLevelStr, message);
}

void Logger::Log(const std::string& message, bool logPosition)
{
    Log(LogLevel::Error, message, logPosition);
}