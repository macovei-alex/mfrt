#include "funcpp.h"

#include <format>

size_t mfrt::GetInt(InfoBlock &info, Logger &logger)
{
    char c;
    size_t num = 0;
    info.GetFrtIn() >> c;
    while (c != '|')
    {
        if (c < '0' || c > '9')
        {
            logger.Log(std::format("Invalid character {} jumped over; expecting a digit or \'|\'\n", c));
            break;
        }
        else
            num = num * 10 + (c - '0');
        logger.currentPosition.second++;
        info.GetFrtIn() >> c;
    }
    return num;
}