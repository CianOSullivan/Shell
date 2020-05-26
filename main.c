#include <stdbool.h>
#include <stdio.h>  // Used by printf and getchar()
#include <stdlib.h> // Used by malloc
#include <string.h> // Used by strtok
#include <unistd.h> // Used by fork
#include <sys/wait.h> // waitpid and its macros
#include "config.h"

int cd(char** args);
int help(char** args);
int shell_exit(char** args);

char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &cd,
  &help,
  &shell_exit
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int cd(char **args)
{
    char cwd[1000];
    if (args[1] == NULL) {
        // Print the error to stderr
        fprintf(stderr, "csh: argument missing\n");
    } else {
        // Attempt to change directory
        if (chdir(args[1]) != 0) {
            // If unsucessful, print the error message
            perror("csh");
        } else {
            printf("Current Directory: %s", getcwd(cwd, sizeof(cwd)));
        }
    }
  return 1;
}

int shell_exit(char** args)
{
  return 0;
}

int help(char** args){
    printf("Possible programs are available: \n");

    for (int i = 0; i < num_builtins(); i++) {
        printf("    %s\n", builtin_str[i]);
    }

    return 1;
}

char* readline() {
    int bufSize = BUFSIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * bufSize);
    char c;

    while (true) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position++] = c;
        }

        if (position >= bufSize) {
            bufSize += BUFSIZE;
            buffer = realloc(buffer, bufSize);
        }
    }

    return buffer;
}

char** splitlines(char* line) {
    int bufSize = ARGSBUFSIZE;
    int position = 0;
    char** arguments = malloc(sizeof(char*) * bufSize);
    char* argument;

    argument = strtok(line, DELIMS);

    while (argument != NULL) {
        arguments[position++] = argument;

        if (position >= bufSize) {
            bufSize += ARGSBUFSIZE;
            arguments = realloc(arguments, sizeof(char*) * bufSize);
        }

        argument = strtok(NULL, DELIMS);
    }

    arguments[position] = NULL;
    return arguments;
}

int launch(char** args) {
    pid_t childID;
    pid_t parentID;
    int status;

    childID = fork();
    if (childID == 0) {
        // Fork successful
        if (execvp(args[0], args) == -1) {
            printf("LS");
        }
    } else if (childID < 0) {
        // Fork unsucessful
        perror("csh");
    } else {
        do {
            parentID = waitpid(childID, &status, WUNTRACED);
            // Wait until process is exited or killed
        }
        while (!WIFSIGNALED(status) && !WIFEXITED(status));
    }

    return 1;
}

bool execute(char** arguments) {
    // Execute
    if (arguments[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(arguments[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(arguments);
        }
    }

    return launch(arguments);
}

int main(int argc, char **argv) {
    bool running = true;
    char* line;
    char** arguments;

    while (running) {
        printf("> ");
        line = readline();
        arguments = splitlines(line);
        running = execute(arguments);

        free(line);
        free(arguments);
    }
    return 0;

}
