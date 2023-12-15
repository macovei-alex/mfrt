#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>

typedef struct control_t
{
    FILE *frtfd, *cfd;
    const char *temp_frt_name, *temp_c_name;
    char *file_name;
    char *options;
} control_t;

typedef enum LogLevel
{
    WARNING,
    ERR
} LogLevel;

int setup(control_t *control);
int before_close(control_t *control);
char *get_options(int argc, char *argv[]);
char *get_file_name(int argc, char *argv[]);
void print(control_t *control, const char *message, size_t tab_count);
void logi(LogLevel level, const char *message);
size_t getInt(control_t *control);
int make_c_code(control_t *control);
int compile_command(control_t *control);
int run(const char *file_name);

#endif // FUNC_H