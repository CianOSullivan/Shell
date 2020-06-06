#ifndef BUILTINS_H
#define BUILTINS_H
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // Used by fork
#include <stdlib.h>

/**
   Change the current directory of the shell.

   @param args the command entered
   @returns true when finished
*/
int cd(char** args);

/**
   Print the help message for builtin functions.

   @param args the command entered
   @returns true when finished
*/
int help(char** args);

/**
   Exit the shell.

   @param args the command entered
   @returns false
 */
int shell_exit(char** args);

/**
   Checks if the given arguments are a builtin function.

   @param arguments the command entered
   @returns true if builtin is matched, false otherwise
*/
int check_builtin(char** arguments);

/**
   Run the builtin function from arguments.

   @param arguments the command entered
   @returns false if the function could not execute.
*/
int run_builtin(char** arguments);
#endif
