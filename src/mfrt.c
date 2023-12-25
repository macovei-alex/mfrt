#include <stdio.h>
#include <windows.h>

#include "..\include\func.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        logMessage(ERR, "Usage: mfrt <filename>");
        return 1;
    }

    InfoUnit info;
    info.inputFRT = getInputFile(argc, argv);
    info.options = getOptions(argc, argv);

    if (setup(&info))
        return 1;

    if (writeC(&info))
    {
        beforeClose(&info);
        return 1;
    }

    if (compileCommand(&info))
    {
        beforeClose(&info);
        return 1;
    }

    if (strstr(info.options, "-r") != NULL)
        if (executeProgram(info.inputFRT))
        {
            beforeClose(&info);
            return 1;
        }

    if (beforeClose(&info))
        return 1;

    return 0;
}