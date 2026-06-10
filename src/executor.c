#include "my_shell.h"

/* How the path actually looks */
/*
    "PATH=/usr/bin:/bin:/sbin" -> separated by :
*/
char *get_path(char **env)
{ // Fetches the PATH environment variable
    // this returns "/usr/bin:/bin:/sbin:/......" the entire string

    for (int i = 0; env[i]; i++)
    {
        if (strncmp(env[i], "PATH=", 5) == 0)
        {
            return strdup(env[i] + 5);
        }
    }
    return NULL;
}

// after getting the entire PATH string now it's time to split them
char **split_path(char *path)
{
    char **dirs = malloc(MAX_INPUT * sizeof(char *)); // string array to store all the dirs

    char *path_copy = strdup(path);     // don't modify original
    char *dir = strtok(path_copy, ":"); // stores one dir at a time

    // strtok(path_copy, ":") -> strtok splits a string by a delimiter. Here the delimiter is :

    // First call - pass the actual string:
    // strtok(path_copy, ":")  // → "/usr/bin"

    int i = 0;
    while (dir)
    {
        dirs[i] = strdup(dir);
        dir = strtok(NULL, ":"); // Every call after - pass NULL (it remembers where it left off)
        i++;
    }

    dirs[i] = NULL; // terminate array
    free(path_copy);
    return dirs;
}

// done with PATH directories
// now time to check whether the given command belongs to the directory or not
char *find_in_path(char *command, char **env)
{
    // if command starts with ./ or / -> use it directly
    if (command[0] == '/' || (command[0] == '.' && command[1] == '/'))
    {
        if (access(command, X_OK) == 0)
            return strdup(command); // use as is
        else
            return NULL; // file not found or not executable
    }

    char *path = get_path(env);     // step 1 -> get the PATH string
    char **dirs = split_path(path); // step 2 -> split it into directories

    for (int i = 0; dirs[i]; i++)
    {
        size_t len = strlen(dirs[i]) + strlen(command) + 2;
        char *full_path = malloc(len);

        // combining the variables as execve takes the full path as the input
        strcpy(full_path, dirs[i]);
        strcat(full_path, "/");
        strcat(full_path, command);

        if (access(full_path, X_OK) == 0)
        {
            // int access(const char *pathname, int mode);
            // returns 0 → file exists and permission granted
            // returns -1 → file doesn't exist or permission denied
            /*
                | Flag   |   Checks            |
                |--------+---------------------|
                | F_OK   | does file exist?    |
                | R_OK   | is file readable?   |
                | W_OK   | is file writable?   |
                | X_OK   | is file executable? |
            */

            // free dirs before returning
            for (int j = 0; dirs[j]; j++)
                free(dirs[j]);
            free(dirs);
            dirs = NULL;

            return full_path;
        }

        free(full_path);
    }

    // if command not found
    for (int i = 0; dirs[i]; i++)
        free(dirs[i]);
    free(dirs);
    return NULL;
}

int executor(char **args, char **env)
{

    // find full path of command
    char *full_path = find_in_path(args[0], env);

    if (!full_path)
    {
        printf("%s: command not found\n", args[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        free(full_path);
        return 1;
    }

    if (pid == 0)
    {
        // child process
        execve(full_path, args, env);
        // execve replaces the current program with a system call
        // that's why we use it inside child process so that the original process keeps on running

        perror("execve");
        exit(EXIT_FAILURE);
    }
    else
    {
        // parent process
        int status;
        waitpid(pid, &status, 0);
        free(full_path);

        if (WIFEXITED(status)) // exited normally
        {

            return WEXITSTATUS(status);
            // WEXITSTATUS(status) = 0     -> success
            // WEXITSTATUS(status) = 1     -> some error
            // WEXITSTATUS(status) = 127   -> command not found
        }
        else if (WIFSIGNALED(status)) // program crashed or was killed (ctrl+c, segfault etc)
        {
            //  child was killed by a signal
            /*
            | Signal  |   Number  |      Cause                    |
            |---------|-----------|-------------------------------|
            | SIGSEGV |    11     | segfault (bad memory access)  |
            | SIGKILL |    9      | force killed                  |
            | SIGINT  |    2      | ctrl+c                        |
            | SIGTERM |    15     | termination request           |
            */
            printf("killed by signal %d\n", WTERMSIG(status));
            return 1;
        }
    }

    return 0;
}