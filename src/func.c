#include "../include/func.h"

#include <string.h>
#include <windows.h>

Logger logger = {
    .row = 0,
    .col = 0,
    .buffer = ""
};

int setup(InfoUnit* info) {
    setPaths(info, false);

    char command[512];
    sprintf(command, "copy %s %s >nul", info->inputFRT, info->tempFRT);

    if (system(command) != 0) {
        logMessage(ERR, "Could not copy the frt file");
        return 1;
    }

    info->frtFD = fopen(info->tempFRT, "rb");
    if (info->frtFD == NULL) {
        logMessage(ERR, "Could not open the copied file");
        return 1;
    }

    info->cFD = fopen(info->tempC, "wb");
    if (info->cFD == NULL) {
        logMessage(ERR, "Could not open the C file");
        fclose(info->frtFD);
        return 1;
    }

    return 0;
}

void setPaths(InfoUnit* info, bool doPrint) {
    char execPath[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, execPath, sizeof(execPath));

    if (doPrint) {
        printf("EXEC_PATH: %s\n", execPath);
    }

    // Remove the executable name to get the directory
    char* lastBackslash;
    if ((lastBackslash = strrchr(execPath, '\\')) != NULL) {
        *lastBackslash = '\0';
    }
    if ((lastBackslash = strrchr(execPath, '\\')) != NULL) {
        *lastBackslash = '\0';
    }

    char tempDir[MAX_PATH];
    sprintf(tempDir, "%s\\temp", execPath);

    if (doPrint) {
        printf("TEMP_DIR: %s\n", tempDir);
    }

    info->tempFRT = malloc(MAX_PATH);
    sprintf(info->tempFRT, "%s\\src.frt", tempDir);

    if (doPrint) {
        printf("TEMP_FRT_PATH: %s\n", info->tempFRT);
    }

    info->tempC = malloc(MAX_PATH);
    sprintf(info->tempC, "%s\\src.c", tempDir);

    if (doPrint) {
        printf("TEMP_C_PATH: %s\n", info->tempC);
    }
}

int beforeClose(InfoUnit* control) {
    if (remove(control->tempFRT) != 0) {
        logMessage(ERR, "Could not delete the temporary frt file");
        return 1;
    }
    if (remove(control->tempC) != 0) {
        logMessage(ERR, "Could not delete the C file");
        return 1;
    }

    free(control->inputFRT);

    return 0;
}

char* getOptions(int argc, char* argv[]) {
    char* options = malloc(MAX_PATH);
    options[0] = '\0';

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            strcat(options, argv[i]);
            strcat(options, " ");
        }
    }

    return options;
}

char* getInputFile(int argc, char* argv[]) {
    char* inputFile = malloc(MAX_PATH);
    inputFile[0] = '\0';

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            strcpy(inputFile, argv[i]);
            break;
        }
    }

    return inputFile;
}

void print(InfoUnit* info, const char* message, size_t tabCount) {
    static int doPrint = -1;
    if (doPrint == -1) {
        doPrint = (strstr(info->options, "-v") != NULL);
    }

    char* messageWithTabs = malloc(strlen(message) + tabCount + 1);
    strcpy(messageWithTabs + tabCount, message);
    for (size_t i = 0; i < tabCount; i++) {
        messageWithTabs[i] = '\t';
    }

    fprintf(info->cFD, messageWithTabs);
    if (doPrint) {
        fprintf(stdout, messageWithTabs);
    }

    free(messageWithTabs);
}

void logMessage(LogLevel level, const char* message) {
    if (level == WARNING) {
        fprintf(stderr, "[Warning]: %s\n", message);
    }
    else if (level == ERR) {
        fprintf(stderr, "[Error]: %s\n", message);
    }
}

size_t getInt(InfoUnit* info) {
    char c;
    size_t num = 0;
    while ((c = fgetc(info->frtFD)) != '|') {
        if (c < '0' || c > '9') {
            sprintf(logger.buffer, "Invalid character %c at row%lld, col:%lld jumped over; expecting a digit or \'|\'\n", c, logger.row, logger.col);
            logMessage(ERR, logger.buffer);
            break;
        }
        else {
            num = num * 10 + (c - '0');
        }
    }
    return num;
}

int writeC(InfoUnit* info) {
    print(info, "#include <stdio.h>\n\n", 0);
    print(info, "#define max (ptr - array > highest ? ptr - array : highest)\n", 0);
    print(info, "#define min (ptr - array < lowest ? ptr - array : lowest)\n\n", 0);
    print(info, "int main()\n{\n\tint array[1024] = {0};\n\tint *ptr = array + 511;\n\n", 0);
    print(info, "\tint lowest = ptr - array, highest = ptr - array;\n", 0);

    char c;
    size_t tabCount = 1;
    size_t num;
    char forLoop[40];

    while ((c = fgetc(info->frtFD)) != EOF) {
        switch (c) {
        case '>':
            print(info, "ptr++; highest = max;\n", tabCount);
            break;
        case '<':
            print(info, "ptr--; lowest = min;\n", tabCount);
            break;
        case '+':
            print(info, "(*ptr)++;\n", tabCount);
            break;
        case '-':
            print(info, "(*ptr)--;\n", tabCount);
            break;
        case '_':
            print(info, "printf(\"%%lld\\n\"*ptr);\n", tabCount);
            break;
        case '@':
            print(info, "for(int i = lowest; i <= highest; i++)\n", tabCount);
            print(info, "printf(\"%%lld \", array[i]);\n", tabCount + 1);
            print(info, "putchar(\'\\n\');\n", tabCount);
            break;
        case '.':
            print(info, "putchar((char)(*ptr));\n", tabCount);
            break;
        case '#':
            print(info, "for(int i = lowest; i <= highest; i++)\n", tabCount);
            print(info, "putchar((char)(array[i]));\n", tabCount + 1);
            break;
        case '\\':
            print(info, "putchar(\'\\n\');\n", tabCount);
            break;
        case ',':
            print(info, "scanf(\"%%d\", ptr);\n", tabCount);
            break;
        case '[':
            num = getInt(info);
            sprintf(forLoop, "for(int i = 0; i < %lld; i++)\n", num);
            print(info, forLoop, tabCount);
            print(info, "{\n", tabCount);
            tabCount++;
            break;
        case ']':
            if (tabCount > 1)
            {
                print(info, "}\n", tabCount - 1);
                tabCount--;
            }
            else
                logMessage(ERR, "Unmatched \']\' jumped over\n");
            break;
        case '/':
            c = fgetc(info->frtFD);
            while (c != '\n' && c != '/' && c != EOF)
                c = fgetc(info->frtFD);
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
            sprintf(logger.buffer, "Invalid character %c at row:%lld, col:%lld jumped over\n", c, logger.row, logger.col);
            logMessage(ERR, logger.buffer);
            break;
        }
        logger.col++;
    }

    while (tabCount > 1) {
        print(info, "}\n", tabCount - 1);
        tabCount--;
        logMessage(ERR, "Unmatched \'[\' closed automatically");
    }

    print(info, "\n\treturn 0;\n}\n", 0);

    fclose(info->frtFD);
    fclose(info->cFD);

    return 0;
}

int compileCommand(InfoUnit* info) {
    char command[512];
    command[0] = '\0';

    char programExec[256];
    strcpy(programExec, info->inputFRT);
    programExec[strlen(programExec) - 4] = '\0';
    strcat(programExec, ".exe");

    strcpy(command, "gcc ");
    strcat(command, "-o ");
    strcat(command, programExec);
    strcat(command, " ");
    strcat(command, info->tempC);

    if (system(command) != 0) {
        logMessage(ERR, "Could not compile the temporary C file");
        return 1;
    }

    return 0;
}

int executeProgram(const char* inputFile) {
    char command[512] = {};

    strcpy(command, inputFile);
    command[strlen(command) - 4] = '\0';

    strcat(command, ".exe");

    if (system(command) != 0) {
        logMessage(ERR, "Could not run the executable");
        return 1;
    }

    return 0;
}