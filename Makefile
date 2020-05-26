all: compile

compile:
	gcc -Wall -g main.c -o csh
# Use -g to generate debug information
# Use -Wall to enable all warnings
