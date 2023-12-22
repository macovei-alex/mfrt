#pragma once

#include <cstdint>
#include <string>
#include <fstream>

class Logger
{
public:
    using Position = std::pair<uint64_t, uint64_t>;
    enum class LogLevel : uint8_t
    {
        Info,
        Warning,
        Error
    };

public:
    Logger(std::ostream &logStream);
    Logger(std::ostream &&logStream);
    Logger(const std::string &logFilePath);
    ~Logger() = default;

    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger &operator=(Logger &&) = delete;

public:
    Position currentPosition;

    void Log(LogLevel level, const std::string &message, bool logPosition = true);
    void Log(const std::string &message, bool logPosition = true);

private:
    std::unique_ptr<std::ostream> logStream;
};
