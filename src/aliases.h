#ifndef ALIASES_H
#define ALIASES_H
#include <stdio.h>
#include <regex.h>    // Used by regular expressions
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
   Check if an alias exists for the given command and return the modified argument list if one
   exists.

   @returns the arguments with the alias if one exists for the given command
   @param arguments the arguments to check for the containing alias
   @param location the location of the alias file
*/
char** find_alias(char** arguments, char* location);

 #endif
