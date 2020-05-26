#include <stdbool.h>
#include <stdio.h>  // Used by printf and getchar()
#include <stdlib.h> // Used by malloc
#include <string.h> // Used by strtok
#include <unistd.h> // Used by fork
#include <sys/wait.h> // waitpid and its macros
#include "config.h"
#include "builtins.h"

char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
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
    int status;

    childID = fork();
    if (childID == 0) {
        // This is run by the child if the fork is successful
        if (execvp(args[0], args) == -1) {
            perror("csh");
        }
        // The child then exits when it is finished
        exit(0);
    } else if (childID < 0) {
        // Fork unsucessful
        perror("csh");
    } else {
        do {
            waitpid(childID, &status, WUNTRACED);
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
    if (check_builtin(arguments)) {
        return run_builtin(arguments);
    }
    return launch(arguments);
}


int main(int argc, char **argv) {
    bool running = true;
    char* line;
    char** arguments;
    char* buffer[100];
    int bufferSize = 0;

    while (running) {
        char* cwd = getcwd(NULL, 0);
        if (strstr(cwd, HOME) != NULL) {
            cwd = replace_str(cwd, HOME, "~");
        }

        printf("%s:%s> ", HOSTNAME, cwd);
        line = readline();
        // Make a history file which the shell uses
        buffer[bufferSize++] = strdup(line);
        arguments = splitlines(line);
        running = execute(arguments);
        for (int i = 0; i < bufferSize; i++) {
            printf("%s", buffer[i]);
        }
        printf("%i", bufferSize);
        free(line);
        free(arguments);
    }
    return 0;

}
