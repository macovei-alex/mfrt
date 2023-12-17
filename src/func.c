#include "../include/func.h"

#include <string.h>
#include <windows.h>

logger_t logger = {0, 0, ""};

int setup(control_t *control)
{
    char command[512];
    sprintf(command, "copy %s %s", control->file_name, control->temp_frt_name);

    if (system(command) != 0)
    {
        logi(ERR, "Could not copy the frt file");
        return 1;
    }

    control->frtfd = fopen(control->temp_frt_name, "rb");
    if (control->frtfd == NULL)
    {
        logi(ERR, "Could not open the copied file");
        return 1;
    }

    control->cfd = fopen(control->temp_c_name, "wb");
    if (control->cfd == NULL)
    {
        logi(ERR, "Could not open the C file");
        fclose(control->frtfd);
        return 1;
    }

    return 0;
}

int before_close(control_t *control)
{
    if (remove(control->temp_frt_name) != 0)
    {
        logi(ERR, "Could not delete the temporary frt file");
        return 1;
    }
    if (remove(control->temp_c_name) != 0)
    {
        logi(ERR, "Could not delete the C file");
        return 1;
    }

    free(control->file_name);

    return 0;
}

char *get_options(int argc, char *argv[])
{
    char *options = malloc(256);
    options[0] = '\0';

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            strcat(options, argv[i]);
            strcat(options, " ");
        }
    }

    return options;
}

char *get_file_name(int argc, char *argv[])
{
    char *file_name = malloc(256);
    file_name[0] = '\0';

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            strcpy(file_name, argv[i]);
            break;
        }
    }

    return file_name;
}

void print(control_t *control, const char *message, size_t tab_count)
{
    static int print_to_stdout = -1;
    if (print_to_stdout == -1)
        print_to_stdout = (strstr(control->options, "-v") != NULL);

    char *messageWithTabs = malloc(strlen(message) + tab_count + 1);
    strcpy(messageWithTabs + tab_count, message);
    for (size_t i = 0; i < tab_count; i++)
        messageWithTabs[i] = '\t';

    fprintf(control->cfd, messageWithTabs);
    if (print_to_stdout)
        fprintf(stdout, messageWithTabs);

    free(messageWithTabs);
}

void logi(LogLevel level, const char *message)
{
    if (level == WARNING)
        fprintf(stderr, "[Warning]: %s\n", message);
    else if (level == ERR)
        fprintf(stderr, "[Error]: %s\n", message);
}

size_t getInt(control_t *control)
{
    char c;
    size_t num = 0;
    while ((c = fgetc(control->frtfd)) != '|')
    {
        if (c < '0' || c > '9')
        {
            sprintf(logger.buffer, "Invalid character %c at row:%d, col:%d jumped over; expecting a digit or \'|\'\n", c, logger.row, logger.col);
            logi(ERR, logger.buffer);
            break;
        }
        else
            num = num * 10 + (c - '0');
    }
    return num;
}

int make_c_code(control_t *control)
{
    print(control, "#include <stdio.h>\n\n", 0);
    print(control, "#define max (ptr - array > highest ? ptr - array : highest)\n", 0);
    print(control, "#define min (ptr - array < lowest ? ptr - array : lowest)\n\n", 0);
    print(control, "int main()\n{\n\tint array[1024] = {0};\n\tint *ptr = array + 511;\n\n", 0);
    print(control, "\tint lowest = ptr - array, highest = ptr - array;\n", 0);

    char c;
    size_t tab_count = 1;
    size_t num;
    char forLoop[40];

    while ((c = fgetc(control->frtfd)) != EOF)
    {
        switch (c)
        {
        case '>':
            print(control, "ptr++; highest = max;\n", tab_count);
            break;
        case '<':
            print(control, "ptr--; lowest = min;\n", tab_count);
            break;
        case '+':
            print(control, "(*ptr)++;\n", tab_count);
            break;
        case '-':
            print(control, "(*ptr)--;\n", tab_count);
            break;
        case '_':
            print(control, "printf(\"%%d\\n\"*ptr);\n", tab_count);
            break;
        case '@':
            print(control, "for(int i = lowest; i <= highest; i++)\n", tab_count);
            print(control, "printf(\"%%d \", array[i]);\n", tab_count + 1);
            print(control, "putchar(\'\\n\');\n", tab_count);
            break;
        case '.':
            print(control, "putchar((char)(*ptr));\n", tab_count);
            break;
        case '#':
            print(control, "for(int i = lowest; i <= highest; i++)\n", tab_count);
            print(control, "putchar((char)(array[i]));\n", tab_count + 1);
        case '\\':
            print(control, "putchar(\'\\n\');\n", tab_count);
            break;
        case ',':
            print(control, "scanf(\"%%d\", ptr);\n", tab_count);
            break;
        case '[':
            num = getInt(control);
            sprintf(forLoop, "for(int i = 0; i < %d; i++)\n", num);
            print(control, forLoop, tab_count);
            print(control, "{\n", tab_count);
            tab_count++;
            break;
        case ']':
            if (tab_count > 1)
            {
                print(control, "}\n", tab_count - 1);
                tab_count--;
            }
            else
                logi(ERR, "Unmatched \']\' jumped over\n");
            break;
        case '/':
            c = fgetc(control->frtfd);
            while (c != '\n' && c != '/' && c != EOF)
                c = fgetc(control->frtfd);
            break;
        case '\n':
            logger.row++;
            break;
        case '\r':
            logger.col = -1;
            break;
        case ' ':
            break;
        default:
            sprintf(logger.buffer, "Invalid character %c at row:%d, col:%d jumped over\n", c, logger.row, logger.col);
            logi(ERR, logger.buffer);
            break;
        }
        logger.col++;
    }
    while (tab_count > 1)
    {
        print(control, "}\n", tab_count - 1);
        tab_count--;
        logi(ERR, "Unmatched \'[\' closed automatically");
    }

    print(control, "\n\treturn 0;\n}\n", 0);

    fclose(control->frtfd);
    fclose(control->cfd);

    return 0;
}

int compile_command(control_t *control)
{
    char command[512];
    command[0] = '\0';

    char exe_file[256];
    strcpy(exe_file, control->file_name);
    exe_file[strlen(exe_file) - 4] = '\0';
    strcat(exe_file, ".exe");

    strcpy(command, "gcc ");
    strcat(command, "-o ");
    strcat(command, exe_file);
    strcat(command, " ");
    strcat(command, control->temp_c_name);

    if (system(command) != 0)
    {
        logi(ERR, "Could not compile the temporary C file");
        return 1;
    }
    return 0;
}

int run(const char *file_name)
{
    char command[512];
    command[0] = '\0';

    strcpy(command, file_name);
    command[strlen(command) - 4] = '\0';

    strcat(command, ".exe");

    if (system(command) != 0)
    {
        logi(ERR, "Could not run the executable");
        return 1;
    }

    return 0;
}