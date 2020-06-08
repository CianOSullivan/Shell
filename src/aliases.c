#include "aliases.h"

/**
   Count the number of arguments in the double pointer.

   @returns the number of arguments in args
   @param args the char** to count
*/
int count_args(char** args) {
    int count = 0;
    while (*args) {
        count++;
        args++;
    }
    return count;
}

/**
   Checks if the alias of the current line matches the command.

   @returns true if the command has an alias which exists for it, false otherwise
   @param command the command which is being checked for in the alias line
   @param line the current line in the alias file
*/
bool run_regex(char* command, char* line) {
    regex_t regex;
    int reti;
    char* start = "alias\\s(";
    char* end = ")\\=";
    char* expression = (char*) malloc (1 + strlen(start) + strlen(command) + strlen(end));

    // Set the regex to the combined expression
    strcpy(expression, start);
    strcat(expression, command); // Add command to middle of regex
    strcat(expression, end);

    // Compile regex and execute it on the current line
    reti = regcomp(&regex, expression, REG_EXTENDED);
    reti = regexec(&regex, line, 0, NULL, 0);
    regfree(&regex);

    // Return true if the line is a match for the current command
    if(!reti){
        return true;
    }
    return false;
}

/**
   Get the contents of the brackets of the alias.

   @returns the contents of the brackets of the alias
   @param line the current line in the alias file
*/
char* get_brackets(char* line) {
    char lineCopy[strlen(line)];  // where we will put a copy of the input
    char* subString; // the "result"

    strcpy(lineCopy, line);

    subString = strtok(lineCopy, "\""); // find the first double quote
    subString = strtok(NULL, "\"");   // find the second double quote

    return subString;
}

/**
   Make the argument list using the alias in the current line

   @returns the modified argument list containing the alias
   @param argc the number of arguments in the original argument list
   @param args the original list of arguments
   @param line the current line in the alias file
 */
char** make_arg_list(int argc, char** args, char* line) {
    int start_argument = 0;   // The position in args which the the alias stops at
    char* alias;              // The alias for args[0]
    // Compare args[0] against alias of current line

    char** modified_args = malloc(sizeof(char *) * 100); // The argument list containing the alias
    char* pch; // The character pointer of the tokenized brackets

    // Get the contents of the brackets and tokenize it
    alias = get_brackets(line);
    pch = strtok(alias, " ");

    // Add alias to modified_args
    while (pch != NULL) {
        modified_args[start_argument] = malloc(sizeof(char) * strlen(alias));
        strcpy(modified_args[start_argument++], pch);
        pch = strtok(NULL, " ");
    }

    int newStart = start_argument; // The point at which to start adding args

    // Add the old arguments into the new alias argument list
    for (int i = 1; i < start_argument - 1 + argc; i++) {
        modified_args[newStart++] = args[i];
    }

    // Return the arguments
    return modified_args;
}

/**
   Check if the alias exists for the current command and return the argument list with the alias
   included if it is found in the alias file.

   @returns the modified args list if alias is found, NULL if not
   @param argc the number of arguments in the char** pointer
   @param args the list of command arguments
   @param location the location of the alias file
*/
char** check_alias(int argc, char** args, char* location) {
    char* line = NULL;        // The current line of the file
    size_t len;               // The length of the getline return

    // Don't parse
    if (argc > 100) {
        printf("Too many arguments");
        return NULL;
    }

    // Open aliases file
    FILE *aliases = fopen(location, "r+");

    // File not found
    if(aliases == NULL) {
        return NULL;
    }

    // Iterate through aliases file
    while ((getline(&line, &len, aliases)) != -1) {
        // Check if the alias on the current line matches the command
        bool match = run_regex(args[0], line);

        // If an alias exists for the command
        if (match) {
            fclose(aliases);
            return make_arg_list(argc, args, line);
        }
    }

    // Return NULL if no match found
    fclose(aliases);
    return NULL;
}

char** find_alias(char** arguments, char* location) {
    int argc = count_args(arguments);

    char** alias = malloc(sizeof(char*) * argc);
    alias = check_alias(argc, arguments, location);

    // Return the alias if one exists for the given command
    if (alias) {
        return alias;
    }

    // Otherwise return the original argument list
    return arguments;
}
