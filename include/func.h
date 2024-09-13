#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef struct InfoUnit {
    FILE* frtFD, * cFD;
    char* tempFRT, * tempC;
    char* inputFRT;
    char* options;
} InfoUnit;

typedef struct Logger {
    size_t row, col;
    char buffer[1024];
} Logger;

typedef enum LogLevel {
    WARNING,
    ERR
} LogLevel;

int setup(InfoUnit* info);
void setPaths(InfoUnit* info, bool doPrint);
int beforeClose(InfoUnit* info);
char* getOptions(int argc, char* argv[]);
char* getInputFile(int argc, char* argv[]);
void print(InfoUnit* info, const char* message, size_t tabCount);
void logMessage(LogLevel level, const char* message);
size_t getInt(InfoUnit* info);
int writeC(InfoUnit* info);
int compileCommand(InfoUnit* info);
int executeProgram(const char* inputFile);
