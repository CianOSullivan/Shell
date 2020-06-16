#include <stdbool.h>   // Allows use of boolean values
#include <stdio.h>     // Used by printf and getchar()
#include <stdlib.h>    // Used by malloc
#include <string.h>    // Used by strtok
#include <unistd.h>    // Used by fork
#include <sys/wait.h>  // waitpid and its macros
#include <signal.h>    // Used by backtracer
#include <execinfo.h>  // Used by backtracer
#include "config.h"    // CSH config values
#include "builtins.h"  // Builtin commands
#include "aliases.h"   // Used by alias parsing

/**
   Print the colour red.
*/
void red () {
    printf("\033[0;31m");
}

/**
   Print the colour white.
*/
void white() {
    printf("\033[0m");
}

/**
   Print the colour green.
*/
void green() {
    printf("\033[0;32m");
}

/**
   Print the prompt with the appropriate colours.

   @param cwd the current working directory
*/
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

/**
   Print a backtrace upon error signal and then exit.

   @param sig the signal error code
*/
void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print the backtrace to stderr
    fprintf(stderr, "Error: signal %i\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

/**
   Replace the home section of the current working directory with ~.

   @param cwd the current working directory of csh
   @param home the home directory of the user
*/
void replace_home(char* cwd, char* home) {
    char* position = strstr(cwd, home);

    // Check to_replace is in original
    if (position) {
        // If HOME exists in original, replace it with ~
        sprintf(cwd, "%s%s", "~", position + strlen(home));
    }
}

/**
   Read the user input.

   @returns the line of user input
*/
char* readline() {
    char *line = NULL;  // The current input line
    size_t bufsize = 0; // allows getline to allocate buffer

    if (getline(&line, &bufsize, stdin) == -1){
        // Exit on end of file
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else  {
            perror("csh");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

/**
   Split the given line into distinct arguments.

   @returns the list of arguments
   @param line the line of user input to split
 */
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

/**
   Launch the given command.

   @returns the success status
   @param args the list of arguments to execute
*/
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
        // Wait for child process to finish
        do {
            waitpid(childID, &status, WUNTRACED);
        }
        while (!WIFSIGNALED(status) && !WIFEXITED(status));
    }

    return 1;
}

/**
   Execute the given arguments.

   @returns true if launched successfully, false otherwise
   @param arguments the arguments to execute
*/
bool execute(char** arguments) {
    // Check if arguments is empty
    if (arguments[0] == NULL) {
        return 1;
    }

    // If a builtin exists, run it
    if (check_builtin(arguments)) {
        return run_builtin(arguments);
    }

    // Otherwise, fork a process
    return launch(arguments);
}

/**
   Write the last command to the history file.

   @param line the current line entered by the user
   @param location the location of the history file
*/
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
        history = fopen(location, "w");
        while (token != NULL) {
            fprintf(history, "%s\n", token);
            token = strtok(NULL, "\n");
        }
    }

    fprintf(history, "%s", line);
    fclose(history);
}

/**
   The main loop of the shell and initialiser.
*/
int main(int argc, char **argv) {
    bool running = true;       // Whether the shell is running or not
    char* line;                // The current line input into csh
    char** arguments;          // The arguments to be executed
    char* cwd;                 // The current working directory of csh
    signal(SIGSEGV, handler);  // Install backtracer

    // Get files required by csh
    char* HOME = getenv("HOME");

    char* alias_location = malloc(strlen(HOME) + 1 + 20);
    strcpy(alias_location, HOME);
    strcat(alias_location, "/.config/csh/aliases");

    char* hist_location = malloc(strlen(HOME) + 1 + 17);
    strcpy(hist_location, HOME);
    strcat(hist_location, "/.config/csh/hist");

    while (running) {
        // Format the current working directory
        cwd = getcwd(NULL, 0);
        //replace_string(cwd, HOME);

        if (strstr(cwd, HOME) != NULL) {
            replace_home(cwd, HOME);
        }
        print_prompt(cwd);                                 // Print the command line promp
        line = readline();                                 // Get the user input
        write_history(line, hist_location);                // Make a history file which the shell uses
        arguments = splitlines(line);                      // Split the input into distinct arguments
        arguments = find_alias(arguments, alias_location); // Check if an alias exists for the given command
        running = execute(arguments);                      // Execute the input commands

        // Free the last line and the input arguments
        free(line);
        free(arguments);
    }
    return 0;
}
