#include "../include/func.h"

#include <string.h>
#include <windows.h>

int setup(int argc, char *argv[], control_t *control)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char command[512];

    strcpy(command, "copy ");
    strcat(command, control->file_name);
    strcat(command, " ");
    strcat(command, control->temp_bf_name);

    if (system(command) != 0)
    {
        fprintf(stderr, "Error copying file\n");
        return 1;
    }

    control->bffd = fopen(control->temp_bf_name, "rb");
    if (control->bffd == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    control->cfd = fopen(control->temp_c_name, "wb");
    if (control->cfd == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        fclose(control->bffd);
        return 1;
    }

    return 0;
}

int before_close(control_t *control)
{
    if (remove(control->temp_bf_name) != 0)
    {
        fprintf(stderr, "Error deleting the temporary bf file\n");
        return 1;
    }
    if (remove(control->temp_c_name) != 0)
    {
        fprintf(stderr, "Error deleting the temporary c file\n");
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

size_t getInt(control_t *control)
{
    char c;
    size_t num = 0;
    while ((c = fgetc(control->bffd)) != '|')
    {
        if (c < '0' || c > '9')
        {
            fprintf(stderr, "Warning: Invalid character jumped over: %c; expecting a digit or \'|\'\n", c);
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

    while ((c = fgetc(control->bffd)) != EOF)
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
            print(control, "printf(\"%%d \", *ptr);\n", tab_count + 1);
            print(control, "putchar(\'\\n\');\n", tab_count);
            break;
        case '.':
            print(control, "putchar((char)(*ptr));\n", tab_count);
            break;
        case '#':
            print(control, "for(int i = lowest; i <= highest; i++)\n", tab_count);
            print(control, "putchar((char)(*ptr));\n", tab_count + 1);
        case '\\':
            print(control, "putchar(\\n);\n", tab_count);
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
                fprintf(stderr, "Warning: Unmatched \']\'\n");
            break;
        case '/':
            c = fgetc(control->bffd);
            while ((c = fgetc(control->bffd)) != '\n')
                ;
            break;
        case '\n':
        case '\r':
            break;
        default:
            fprintf(stderr, "Warning: Invalid character: %c\n", c);
            break;
        }
    }
    while (tab_count > 1)
    {
        print(control, "}\n", tab_count - 1);
        tab_count--;
        fprintf(stderr, "Warning: Unmatched \'[\' closed automatically\n");
    }

    print(control, "\n\treturn 0;\n}\n", 0);

    fclose(control->bffd);
    fclose(control->cfd);

    return 0;
}

int compile_command(control_t *control)
{
    char command[512];
    command[0] = '\0';

    char exe_file[256];
    strcpy(exe_file, control->file_name);
    exe_file[strlen(exe_file) - 3] = '\0';
    strcat(exe_file, ".exe");

    strcpy(command, "gcc ");
    strcat(command, "-o ");
    strcat(command, exe_file);
    strcat(command, " ");
    strcat(command, control->temp_c_name);

    if (system(command) != 0)
    {
        fprintf(stderr, "Error compiling the temporary c file\n");
        return 1;
    }
    return 0;
}

int run(const char *file_name)
{
    char command[512];
    command[0] = '\0';

    strcpy(command, file_name);
    command[strlen(command) - 3] = '\0';

    strcat(command, ".exe ");

    if (system(command) != 0)
    {
        fprintf(stderr, "Error running the temporary c file\n");
        return 1;
    }

    return 0;
}