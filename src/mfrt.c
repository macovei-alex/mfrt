#include <stdio.h>
#include <windows.h>

#include "..\include\func.h"

int main(int argc, char *argv[])
{
    control_t control = {
        NULL,
        NULL,
        "temp\\src.bf",
        "temp\\src.c",
        get_file_name(argc, argv),
        get_options(argc, argv)};

    if (setup(argc, argv, &control))
        return 1;

    if (make_c_code(&control))
    {
        before_close(&control);
        return 1;
    }

    if (compile_command(&control))
    {
        before_close(&control);
        return 1;
    }

    if (strstr(control.options, "-r") != NULL)
        if (run(control.file_name))
        {
            before_close(&control);
            return 1;
        }

    if (before_close(&control))
        return 1;

    return 0;
}