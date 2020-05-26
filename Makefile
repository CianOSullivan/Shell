all: compile

compile:
	gcc -Wall -g main.c builtins.c -o csh
# Use -g to generate debug information
# Use -Wall to enable all warnings
