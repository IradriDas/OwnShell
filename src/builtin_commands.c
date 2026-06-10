#include "my_shell.h"

int command_help()
{
    printf("=========================================\n");
    printf("        My Shell - by Iradri\n");
    printf("=========================================\n\n");

    printf("BUILT-IN COMMANDS:\n");
    printf("  cd [dir]              Change directory\n");
    printf("  pwd                   Print working directory\n");
    printf("  echo [text]           Print text\n");
    printf("  touch <file>          Create file(s)\n");
    printf("  mkdir <dir>           Create directory(s)\n");
    printf("  date                  Show current date and time\n");
    printf("  env                   Show environment variables\n");
    printf("  setenv VAR=value      Create environment variable\n");
    printf("  setenv <VAR> <value>  Create environment variable\n");
    printf("  unsetenv VAR          Remove environment variable\n");
    printf("  help                  Show this help message\n");
    printf("  exit or quit          Exit the shell\n\n");

    printf("ECHO FLAGS:\n");
    printf("  echo -n           Suppress trailing newline\n");
    printf("  echo -e           Enable escape sequences\n");
    printf("  echo -ne          Combine both options\n\n");

    printf("SUPPORTED ESCAPES:\n");
    printf("  \\n                New line\n");
    printf("  \\t                Tab\n");
    printf("  \\\\                Backslash\n");
    printf("  \\r                Carriage return\n");
    printf("  \\a                Alert/Bell\n\n");

    printf("REDIRECTION:\n");
    printf("  echo text > file.txt    Write to file\n");
    printf("  echo text >> file.txt   Append to file\n\n");

    printf("EXTERNAL COMMANDS:\n");
    printf("  Any executable available in PATH\n");
    printf("  Examples: ls, cat, grep, gcc, make\n\n");

    return 0;
}

int command_cd(char **args, char *initial_directory)
{
    if (args[1] == NULL)
    {
        if (chdir(initial_directory) != 0)
        { // go back to starting directory
            perror("cd");
        }
    }
    else if (chdir(args[1]) == 0)
    {
        // printf("cd worked!\n");
    }
    else
    {
        perror("cd");
    }
    return 0;
}

int command_pwd()
{
    char *cwd = getcwd(NULL, 0);

    if (cwd != NULL)
    {
        printf("%s\n", cwd);
        free(cwd);
        cwd = NULL;
    }
    else
    {
        perror("getcwd() error");
        return 1;
    }
    return 0;
}

int command_echo(char **args, char **env)
{
    (void)env;

    if (args[1] == NULL)
    {
        printf("\n");
        return 0;
    }

    // --- parse flags ---
    int no_newline = 0; // -n flag
    int escape_on = 0;  // -e flag
    int start = 1;      // index where actual text starts

    for (int i = 1; args[i]; i++) // checks for all the flags
    {
        if (strcmp(args[i], "-n") == 0)
        {
            no_newline = 1;
            start++;
        }
        else if (strcmp(args[i], "-e") == 0)
        {
            escape_on = 1;
            start++;
        }
        else if (strcmp(args[i], "-ne") == 0 || strcmp(args[i], "-en") == 0)
        {
            no_newline = 1;
            escape_on = 1;
            start++;
        }
        else
        {
            break; // not a flag, stop scanning
        }
    }

    // --- scan for redirection operator ---
    char *filename = NULL;
    char *mode = NULL;
    int text_end_index = -1;

    for (int i = start; args[i]; i++)
    {
        if (strcmp(args[i], ">>") == 0)
        {
            mode = "a";
            filename = args[i + 1];
            text_end_index = i;
            break;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            mode = "w";
            filename = args[i + 1];
            text_end_index = i;
            break;
        }
    }

    // --- helper: print with escape handling ---
    FILE *out = (filename && mode) ? fopen(filename, mode) : NULL;

    if (filename && mode && out == NULL)
    {
        perror("echo");
        return 1;
    }

    FILE *target = out ? out : stdout; // write to file or terminal

    for (int i = start; args[i] && (text_end_index == -1 || i < text_end_index); i++)
    {
        if (escape_on)
        {
            for (int j = 0; args[i][j] != '\0'; j++)
            {
                if (args[i][j] == '\\')
                {
                    j++; // look at next character
                    switch (args[i][j])
                    {
                    case 'n':
                        fprintf(target, "\n");
                        break;
                    case 't':
                        fprintf(target, "\t");
                        break;
                    case '\\':
                        fprintf(target, "\\");
                        break;
                    case 'a':
                        fprintf(target, "\a");
                        break;
                    case 'r':
                        fprintf(target, "\r");
                        break;
                    default:
                        fprintf(target, "\\%c", args[i][j]);
                        break;
                    }
                }
                else
                {
                    fprintf(target, "%c", args[i][j]);
                }
            }
        }
        else
        {
            fprintf(target, "%s", args[i]);
        }

        if (args[i + 1] && (text_end_index == -1 || i + 1 < text_end_index))
            fprintf(target, " "); // space between words
    }

    if (!no_newline)
        fprintf(target, "\n"); // -n suppresses this

    if (out)
        fclose(out);

    return 0;
}

int command_touch(char **args)
{
    if (args[1] == NULL)
    {
        printf("touch: missing file operand\n");
        return 1;
    }

    for (int i = 1; args[i]; i++)
    {
        char *filename = args[i];

        size_t len = strlen(filename);

        if (len >= 2 && (filename[0] == '"' || filename[len - 1] == '"'))
        {
            if (filename[len - 1] == '"')
            {
                filename[len - 1] = '\0'; // Remove ending quote
            }

            if (filename[0] == '"')
            {
                filename++; // Skip starting quote
            }
        }

        FILE *fptr = fopen(filename, "a");

        if (fptr == NULL)
        {
            perror("touch");
            return 1;
        }

        fclose(fptr);
    }

    return 0;
}

int command_mkdir(char **args)
{
    if (args[1] == NULL)
    {
        printf("mkdir: missing operand\n");
        return 1;
    }

    for (int i = 1; args[i]; i++)
    {
        char *dir_name = args[i];

        size_t len = strlen(dir_name);

        if (len >= 2 && (dir_name[0] == '"' || dir_name[len - 1] == '"'))
        {
            if (dir_name[len - 1] == '"')
            {
                dir_name[len - 1] = '\0'; // Remove ending quote
            }

            if (dir_name[0] == '"')
            {
                dir_name++; // Skip starting quote
            }
        }

        if (make_folder(dir_name) != 0)
        {
            perror("mkdir");
        }
    }

    return 0;
}

int command_date()
{
    time_t raw_time;
    struct tm *current_time;

    // Get current raw system time
    time(&raw_time);

    // Convert raw time to local time structure
    current_time = localtime(&raw_time);

    int hour = (current_time->tm_hour) % 12;
    if (hour == 0)
    {
        hour = 12;
    }

    int minute = current_time->tm_min;
    int sec = current_time->tm_sec;
    char *am_pm = (current_time->tm_hour >= 12) ? "PM" : "AM";

    char *day = "unknown";
    switch (current_time->tm_wday)
    {
    case 0:
        day = "Sunday";
        break;

    case 1:
        day = "Monday";
        break;

    case 2:
        day = "Tuesday";
        break;

    case 3:
        day = "Wednesday";
        break;

    case 4:
        day = "Thursday";
        break;

    case 5:
        day = "Friday";
        break;

    case 6:
        day = "Saturday";
        break;

    default:
        break;
    }

    char *month = "unknown";
    switch (current_time->tm_mon)
    {
    case 0:
        month = "Jan";
        break;

    case 1:
        month = "Feb";
        break;
    case 2:
        month = "Mar";
        break;
    case 3:
        month = "Apr";
        break;
    case 4:
        month = "May";
        break;
    case 5:
        month = "Jun";
        break;
    case 6:
        month = "Jul";
        break;
    case 7:
        month = "Aug";
        break;
    case 8:
        month = "Sep";
        break;
    case 9:
        month = "Oct";
        break;
    case 10:
        month = "Nov";
        break;
    case 11:
        month = "Dec";
        break;

    default:
        break;
    }
    int date = current_time->tm_mday;

    int year = current_time->tm_year + 1900;

    printf("%s, %s %d, %d, %02d:%02d:%02d %s \n", day, month, date, year, hour, minute, sec, am_pm);

    return 0;
}

int command_env(char **env)
{
    if (!env)
    {
        printf("env: no environment found\n");
        return 1;
    }

    size_t index = 0;
    while (env[index])
    {
        printf("%s\n", env[index]);
        index++;
    }
    return 0;
}

// helper function to count no. of environment variables
int count_env_vars(char **env)
{
    int count = 0;
    while (env[count])
    {
        count++;
    }

    return count;
}

// function to set an environment variable
char **command_setenv(char **args, char **env)
{
    if (args[1] == NULL)
    {
        printf("Usage:\tsetenv VAR=value\nor\tsetenv <variable> <value>\n");
        return env;
    }

    int env_count = count_env_vars(env);
    char **new_env = malloc((env_count + 2) * sizeof(char *));

    if (new_env == NULL)
    {
        perror("malloc");
        return env;
    }

    // copy the existing env variables
    for (size_t i = 0; i < env_count; i++)
    {
        new_env[i] = strdup(env[i]);
        if (new_env[i] == NULL)
        {
            perror("strdup");
            for (size_t j = 0; j < i; j++)
            {
                free(new_env[j]);
            }
            free(new_env);
            return env;
        }
    }

    char *new_var = NULL;
    if (args[2] == NULL)
    {
        new_var = strdup(args[1]);
    }
    else
    {
        new_var = malloc(strlen(args[1]) + strlen(args[2]) + 2);
        if (new_var)
        {
            sprintf(new_var, "%s=%s", args[1], args[2]);
        }
    }

    if (new_var == NULL)
    {
        perror("malloc");
        for (int i = 0; i < env_count; i++)
        {
            free(new_env[i]);
        }
        free(new_env);
        return env;
    }

    new_env[env_count] = new_var;
    new_env[env_count + 1] = NULL;

    printf("new var = %s\n", new_var);
    return new_env;
}

// function to set an environment variable
char **command_unsetenv(char **args, char **env)
{
    if (args[1] == NULL)
    {
        printf("Usage:\tunsetenv <variable>\n");
        return env;
    }

    int env_count = count_env_vars(env);
    char **new_env = malloc(env_count * sizeof(char *));
    if (new_env == NULL)
    {
        perror("malloc");
        return env;
    }

    int found = 0, j = 0;
    int target_var_size = strlen(args[1]);
    for (size_t i = 0; env[i]; i++)
    {
        if (strncmp(env[i], args[1], target_var_size) == 0 && env[i][target_var_size] == '=')
        {
            found = 1;
            free(env[i]);
        }
        else
        {
            new_env[j] = env[i];
            j++;
        }
    }

    if (!found)
    {
        printf("unsetenv: variable %s not found\n", args[1]);
        free(new_env);
        return env;
    }

    return new_env;
}