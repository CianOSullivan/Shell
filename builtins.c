#include "builtins.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // Used by fork
#include <ncurses.h>


char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};


int cd(char** args);
int help(char** args);
int shell_exit(char** args);

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
    if (args[1] == NULL) {
        // Print the error to stderr
        printw("csh: argument missing\n");
    } else {
        // Attempt to change directory
        if (chdir(args[1]) != 0) {
            // If unsucessful, print the error message
            perror("csh");
        }
    }
  return 1;
}

int shell_exit(char** args)
{
  return 0;
}

int help(char** args){
    printw("Possible programs are available: \n");

    for (int i = 0; i < num_builtins(); i++) {
        printw("    %s\n", builtin_str[i]);
    }

    return 1;
}

int check_builtin(char** arguments) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(arguments[0], builtin_str[i]) == 0) {
            return true;
        }
    }
    return false;
}

int run_builtin(char** arguments) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(arguments[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(arguments);
        }
    }
    return 0;
}
