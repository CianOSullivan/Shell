#ifndef BUILTINS_H
#define BUILTINS_H


int cd(char** args);
int help(char** args);
int shell_exit(char** args);
int check_builtin(char** arguments);
int run_builtin(char** arguments);
#endif
