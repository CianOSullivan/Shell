#include "builtins.h"

/**
   The names of the builtin functions.
*/
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

/**
   The builtin function pointers.
*/
int (*builtin_func[]) (char **) = {
  &cd,
  &help,
  &shell_exit
};

/**
   The number of builtin functions.

   @returns the number of builtin functions
*/
int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int cd(char **args)
{
    if (args[1] == NULL) {
        // Change to $HOME
        char* HOME = getenv("HOME");
        if (chdir(HOME) != 0) {
            // If unsucessful, print the error message
            perror("csh");
        }

    } else {
        // Attempt to change directory using given argument
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
    printf("Possible programs are available: \n");

    // Iterate through and print the list of builtins
    for (int i = 0; i < num_builtins(); i++) {
        printf("    %s\n", builtin_str[i]);
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
