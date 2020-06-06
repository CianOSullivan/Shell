#ifndef ALIASES_H
#define ALIASES_H
#include <stdio.h>
#include <regex.h>    // Used by regular expressions
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
   Check if the alias exists for the current command and return the argument list with the alias
   included if it is found in the alias file.

   @returns the modified args list if alias is found, NULL if not
   @param argc the number of arguments in the char** pointer
   @param args the list of command arguments
   @param location the location of the alias file
*/
char** check_alias(int argc, char** args, char* location);

 #endif
