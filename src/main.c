#include <stdbool.h>
#include <stdio.h>  // Used by printf and getchar()
#include <stdlib.h> // Used by malloc
#include <string.h> // Used by strtok
#include <unistd.h> // Used by fork
#include <sys/wait.h> // waitpid and its macros
#include <signal.h>
#include <execinfo.h>
#include "config.h"
#include "builtins.h"
#include "aliases.h"
//extern char* HOME;
/**
Print the colour red
 */
void red () {
    printf("\033[0;31m");
}

/**
Print the colour white
 */
void white() {
    printf("\033[0m");
}

/**
Print the colour green
 */
void green() {
    printf("\033[0;32m");
}

void print_prompt(char* cwd) {
    red();
    printf("%s", HOSTNAME);
    white();
    printf(":");
    green();
    printf("%s", cwd);
    white();
    printf("> ");
}

void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

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
    char *line = NULL;
    size_t bufsize = 0; // have getline allocate a buffer for us

    if (getline(&line, &bufsize, stdin) == -1){
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);  // We recieved an EOF
        } else  {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }

    return line;

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
            fprintf(stderr, "csh: command not found: %s\n", args[0]);
        }
        // The child then exits when it is finished
        exit(1);
    } else if (childID < 0) {
        // Fork unsucessful
        perror("csh");
    } else {
        //fprintf(stderr, "Executed %s\n", args[0]);

        do {
            waitpid(childID, &status, WUNTRACED);
        }
        while (!WIFSIGNALED(status) && !WIFEXITED(status));
    }

    return 1;
}

int count_args(char** args) {
    int count = 0;
    while (*args) {
        count++;
        args++;
    }
    return count;
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

char** find_alias(char** arguments, char* location) {
    int argc = count_args(arguments);

    char** alias = malloc(sizeof(char*) * argc);
    alias = check_alias(argc, arguments, location);
    if (alias) {
        return alias;
    }
    return arguments;
}

void write_history(char* line, char* location) {
    // Open the file for read and append, make if it does not exist
    FILE *history = fopen(location,"a+");
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

    fprintf(history, "%s", line);
    fclose(history);
}

int main(int argc, char **argv) {
    bool running = true;
    char* line;
    char** arguments;

    char* HOME = getenv("HOME");
    char* alias_location = strdup(HOME);
    char* hist_location = strdup(HOME);
    strcat(alias_location, "/.aliases");
    strcat(hist_location, "/.hist");


    signal(SIGSEGV, handler);   // install our handler
    while (running) {
        char* cwd = getcwd(NULL, 0);
        if (strstr(cwd, HOME) != NULL) {
            cwd = replace_str(cwd, HOME, "~");
        }
        print_prompt(cwd);
        line = readline();
        // Make a history file which the shell uses
        write_history(line, hist_location);
        arguments = splitlines(line);
        arguments = find_alias(arguments, alias_location);
        running = execute(arguments);

        free(line);
        free(arguments);
    }
    return 0;

}
