#include "..\include\funcpp.h"

#include <iostream>
#include <format>

size_t mfrt::GetInt(InfoBlock& info, Logger& logger)
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

void mfrt::Print(InfoBlock& info, const std::string& message, size_t tabCount)
{
    static int doPrint = -1;
    if (doPrint == -1)
        doPrint = (info.GetOptions().find("-v") != std::string::npos);

    std::string messageWithTabs{ message };
    for (size_t i = 0; i < tabCount; i++)
        messageWithTabs.insert(0, "\t");

    info.GetCOut() << messageWithTabs;
    if (doPrint)
        std::cout << messageWithTabs;
}

int mfrt::WriteCpp(InfoBlock& info, Logger& logger)
{
    Print(info, "#include <stdio.h>\n\n", 0);
    Print(info, "#define max (ptr - array > highest ? ptr - array : highest)\n", 0);
    Print(info, "#define min (ptr - array < lowest ? ptr - array : lowest)\n\n", 0);
    Print(info, "int main()\n{\n\tint array[1024] = {0};\n\tint *ptr = array + 511;\n\n", 0);
    Print(info, "\tint lowest = ptr - array, highest = ptr - array;\n", 0);

    char c;
    size_t tabCount = 1;
    size_t num;
    std::string forLoop;

    info.GetFrtIn() >> c;

    while (c != EOF)
    {
        switch (c)
        {
        case '>':
            Print(info, "ptr++; highest = max;\n", tabCount);
            break;
        case '<':
            Print(info, "ptr--; lowest = min;\n", tabCount);
            break;
        case '+':
            Print(info, "(*ptr)++;\n", tabCount);
            break;
        case '-':
            Print(info, "(*ptr)--;\n", tabCount);
            break;
        case '_':
            Print(info, "Printf(\"%%d\\n\"*ptr);\n", tabCount);
            break;
        case '@':
            Print(info, "for(int i = lowest; i <= highest; i++)\n", tabCount);
            Print(info, "Printf(\"%%d \", array[i]);\n", tabCount + 1);
            Print(info, "putchar(\'\\n\');\n", tabCount);
            break;
        case '.':
            Print(info, "putchar((char)(*ptr));\n", tabCount);
            break;
        case '#':
            Print(info, "for(int i = lowest; i <= highest; i++)\n", tabCount);
            Print(info, "putchar((char)(array[i]));\n", tabCount + 1);
        case '\\':
            Print(info, "putchar(\'\\n\');\n", tabCount);
            break;
        case ',':
            Print(info, "scanf(\"%%d\", ptr);\n", tabCount);
            break;
        case '[':
            num = GetInt(info, logger);
            forLoop = std::move(std::format("for(size_t i = 0; i < {}; i++)\n", num));
            Print(info, forLoop, tabCount);
            Print(info, "{\n", tabCount);
            tabCount++;
            break;
        case ']':
            if (tabCount > 1)
            {
                Print(info, "}\n", tabCount - 1);
                tabCount--;
            }
            else
                logger.Log(Logger::LogLevel::Error, "Unmatched \']\' jumped over\n");
            break;
        case '/':
            info.GetFrtIn() >> c;
            while (c != '\n' && c != '/' && c != EOF)
                info.GetFrtIn() >> c;
            break;
        case '\n':
            logger.currentPosition.first++;
            break;
        case '\r':
            logger.currentPosition.second = -1;
            break;
        case ' ':
            break;
        default:
            logger.Log(Logger::LogLevel::Error, std::format("Invalid character {} jumped over\n", c));
            break;
        }
        logger.currentPosition.second++;
        info.GetFrtIn() >> c;
    }
    while (tabCount > 1)
    {
        Print(info, "}\n", tabCount - 1);
        tabCount--;
        logger.Log(Logger::LogLevel::Error, "Unmatched \'[\' closed automatically");
    }

    Print(info, "\n\treturn 0;\n}\n", 0);

    return 0;
}

int mfrt::CompileCommand(InfoBlock& info)
{
    std::string programExec{ std::move(info.GetExecName()) };
    std::string command{ std::format("g++ -o {} {}", info.GetInputFrt(), programExec) };

    if (system(command.c_str()) != 0)
        throw std::exception{ "Could not compile the temporary C++ file" };

    return 0;
}

int mfrt::ExecuteProgram(InfoBlock& info)
{
    std::string command{ std::move(info.GetExecName()) };

    if (system(command.c_str()) != 0)
        throw std::exception{ "Could not run the executable" };

    return 0;
}
