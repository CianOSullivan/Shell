#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024
#define ARGSBUFSIZE 64
#define DELIMS " \t\r\n\a"

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

bool execute(char** arguments) {
    // Execute
}

void loop() {
    bool running = true;
    char* line;
    char** arguments;

    while (running) {
        printf("> ");
        line = readline();
        arguments = splitlines(line);
        running = execute(arguments);
        printf("String entered: %s\n", line);
    }

}

int main(int argc, char **argv) {
    loop();
    return 0;

}
