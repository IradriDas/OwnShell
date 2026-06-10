#include "my_shell.h"

// Parses raw input string into an array of string tokens (words)
// Returns char** where each element points to one word, ending with NULL
char **parse_input(char *input)
{
    size_t buffer_size = MAX_INPUT;

    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token = NULL;

    size_t position = 0;
    size_t token_length = 0;

    if (!tokens)
    { // if tokens is NULL (malloc failed)
        perror("Malloc");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; input[i]; i++)
    {
        if (input[i] == ' ' || input[i] == '\n' || input[i] == '\t' || input[i] == '\r' || input[i] == '\a' )
        { // remove trailing and leading whitespaces
            continue;
        }

        token = &input[i];

        while (input[i] && input[i] != ' ' && input[i] != '\n' && input[i] != '\t' && input[i] != '\r' && input[i] != '\a')
        { // splitting the token and calculating the token_length
            token_length++;
            i++;
        }

        tokens[position] = malloc((token_length + 1) * sizeof(char));

        if (!tokens[position])
        {
            perror("Malloc");
            exit(EXIT_FAILURE);
        }

        for (size_t j = 0; j < token_length; j++)
        { // this loop copying the token char by char to tokens array
            tokens[position][j] = token[j];
        }
        tokens[position][token_length] = '\0'; // null terminate token

        position++;
        token_length = 0; // reset for next token
    }

    tokens[position] = NULL; // TERMINATE the array with null

    return tokens;
}

void free_tokens(char **tokens)
{
    if (!tokens)
    {
        return;
    }

    for (size_t i = 0; tokens[i]; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}