#include "my_shell.h"

// Main components
/*
    shell loop
    input parshing
    command execution
    built-in commands
    external commands
    manage environment variables
    manage path
    error handling
*/

int shell_builts(char **args, char **env, char *initial_directory)
{ // cd pwd echo env setenv unsetenv exit

    if (strcmp(args[0], "cd") == 0)
    {
        return command_cd(args, initial_directory);
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        return command_pwd();
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        return command_echo(args, env);
    }
    else if (strcmp(args[0], "touch") == 0)
    {
        return command_touch(args);
    }
    else if (strcmp(args[0], "mkdir") == 0)
    {
        return command_mkdir(args);
    }
    else if (strcmp(args[0], "date") == 0)
    {
        return command_date();
    }
    else if (strcmp(args[0], "env") == 0)
    {
        return command_env(env);
    }
    else if (strcmp(args[0], "help") == 0)
    {
        return command_help();
    }
    else if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Not a built-in
        return executor(args, env);
    }
}

void shell_loop(char **env)
{
    char *input = NULL;
    size_t input_size = 0;

    char **args;
    char *initial_directory = getcwd(NULL, 0);
    // getcwd sees NULL -> allocates memory automatically using malloc internally
    // 0 tells it -> figure out the size yourself
    // Returns a pointer to the newly allocated string containing the current path

    while (1)
    {
        printf("[iradri@iru %s]:~$ ", getcwd(NULL, 0));
        if (getline(&input, &input_size, stdin) == -1)
        {
            // getline returns -1 in case of
            /*
                end of file (EOD)
                in case of terminal -> ctrl + d
            */
            perror("getline");
            break;
        }

        // parshing input
        args = parse_input(input);

        if (strcmp(args[0], "setenv") == 0)
        {
            env = command_setenv(args, env);
        }
        else if (strcmp(args[0], "unsetenv") == 0)
        {
            env = command_unsetenv(args, env);
        }
        else if (args[0] != NULL)
        {
            shell_builts(args, env, initial_directory);
        }
    }

    free_tokens(args);
}

int main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;

    printf(" _____               _      _ _____           __  ___   \n");
    printf("|_   _|             | |    (_)  __ \\         /_ |/ _ \\  \n");
    printf("  | |  _ __ __ _  __| |_ __ _| |  | | __ _ ___| | (_) | \n");
    printf("  | | | '__/ _` |/ _` | '__| | |  | |/ _` / __| |> _ <  \n");
    printf(" _| |_| | | (_| | (_| | |  | | |__| | (_| \\__ \\ | (_) | \n");
    printf("|_____|_|  \\__,_|\\__,_|_|  |_|_____/ \\__,_|___/_|\\___/  \n\n");

    shell_loop(env);

    return 0;
}