#include <stdbool.h>
#include <stdio.h>  // Used by printf and getchar()
#include <stdlib.h> // Used by malloc
#include <string.h> // Used by strtok
#include <unistd.h> // Used by fork
#include <sys/wait.h> // waitpid and its macros
#include <ncurses.h>
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
    int c;
    while (true) {
        c = getch();

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
        exit(1);
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

void write_history(char* line) {
    // Open the file for read and append, make if it does not exist
    FILE *history = fopen("hist","a+");
    int count = 0;
    char *buffer = NULL;

    // Count the number of lines in the file
    for (char c = getc(history); c != EOF; c = getc(history)) {
        // Increment count if this character is newline
        if (c == '\n') {
            count = count + 1;
        }
    }

    if (count >= HISTLEN) {
        if (fseek(history, 0L, SEEK_END) == 0) {
            /* Get the size of the file. */
            long bufsize = ftell(history);
            if (bufsize == -1) { /* Error */ }

            /* Allocate our buffer to that size. */
            buffer = malloc(sizeof(char) * (bufsize + 1));

            /* Go back to the start of the file. */
            if (fseek(history, 0L, SEEK_SET) != 0) { /* Error */ }

            /* Read the entire file into memory. */
            size_t newLen = fread(buffer, sizeof(char), bufsize, history);
            if ( ferror( history ) != 0 ) {
                fputs("Error reading file", stderr);
            } else {
                buffer[newLen++] = '\0'; /* Just to be safe. */
            }
        }

        fclose(history);

        char *token = strtok(buffer, "\n"); // Split the first line
        token = strtok(NULL, "\n"); // Start adding tokens from second line

        // Overwrite file without first line
        history = fopen("hist","w");
        while (token != NULL) {
            fprintf(history, "%s\n", token);
            token = strtok(NULL, "\n");
        }
    }

    fprintf(history, "%s\n", line);
    fclose(history);
}

int main(int argc, char **argv) {
    bool running = true;
    char* line;
    char** arguments;

    initscr();
    while (running) {
        refresh();
        char* cwd = getcwd(NULL, 0);
        if (strstr(cwd, HOME) != NULL) {
            cwd = replace_str(cwd, HOME, "~");
        }

        printw("%s:%s> ", HOSTNAME, cwd);
        line = readline();
        // Make a history file which the shell uses
        write_history(line);
        arguments = splitlines(line);

        running = execute(arguments);
        //printw("\n");
        free(line);
        free(arguments);
    }
    endwin();
    return 0;

}
