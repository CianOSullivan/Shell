#include "aliases.h"
#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool run_regex(char* command, char* line) {
    regex_t regex;
    int reti;
    char* start = "alias\\s(";
    char* end = ")\\=";

    char* expression = (char*) malloc (1 + strlen(start) + strlen(command) + strlen(end));
    strcpy(expression, start);
    strcat(expression, command);
    strcat(expression, end);

    reti = regcomp(&regex, expression, REG_EXTENDED);
    reti = regexec(&regex, line, 0, NULL, 0);
    regfree(&regex);
    if( !reti ){
        return true;
    }
    return false;
}

/**
Get the contents of the brackets of the alias
 */
char* get_brackets(char* line) {
    char lineCopy[strlen(line)];  // where we will put a copy of the input
    char* subString; // the "result"

    strcpy(lineCopy, line);

    subString = strtok(lineCopy, "\""); // find the first double quote
    subString = strtok(NULL, "\"");   // find the second double quote

    return subString;
}

char** check_alias(char** args) {
    char* alias;
    char* line = NULL;
    size_t len = 0;
    char** modified_args = malloc(sizeof(char *));

    // open file
    FILE *aliases = fopen("src/aliases","r");
    if(aliases == NULL)
    {
        fprintf(stderr, "csh: Couldn't open aliases file\n");
        return args;
    }

    // Compare args[0] against all aliases
    while ((getline(&line, &len, aliases)) != -1) {
        //printf("%s\n", line);
        bool match = run_regex(args[0], line);
        if (match) {
            alias = get_brackets(line);
            //printf("Alias: %s\n", alias);

            // change arg[0] to alias
            modified_args[0] = malloc(sizeof(char) * strlen(alias));
            strcpy(modified_args[0], alias);
            fclose(aliases);
            return modified_args;
        }
    }

    fclose(aliases);
    //close file
    printf("Returning NULL\n");
    return NULL;
}
