#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// check for the OS
#ifdef _WIN32
    #include <direct.h>
    #define make_folder(name) _mkdir(name)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #define make_folder(name) mkdir(name, 0777)
#endif

#define MAX_INPUT 1024
#define MAX_PATH 256

char **parse_input(char *input);
void free_tokens(char **tokens);

int shell_builts(char **args, char **env, char *initial_directory);

// Buit-in functions implementations
int command_cd(char **args, char *initial_directory);
int command_pwd();
int command_echo(char **args, char **env);
int command_env(char **env);
int command_touch(char **args);
int command_mkdir(char **args);
int command_date();
int command_help();

int executor(char** args, char** env);
int child_process(char** args, char** env);

char** command_setenv(char **args, char **env);
char** command_unsetenv(char **args, char **env);

// helper functions
char *strdup(const char *str);