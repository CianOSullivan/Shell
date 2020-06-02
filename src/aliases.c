#include "aliases.h"
#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

void run_regex(char* command, char* line) {
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
        printf("Match\n");
    }

}

char** check_alias(char** args) {
    char** alias;
    char * line = NULL;
    size_t len = 0;


    // open file
    FILE *aliases = fopen("src/aliases","r");
    if(aliases == NULL)
    {
        fprintf(stderr, "csh: Couldn't open aliases file\n");
        return args;
    }

    // Compare args[0] against all aliases
    while ((getline(&line, &len, aliases)) != -1) {
        printf("%s\n", line);
        run_regex(args[0], line);
    }
    //if match then return contents of brackets of alias
    //close file

    // else return none
    fclose(aliases);
    //close file
    return alias;
}
