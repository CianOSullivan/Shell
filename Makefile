all: compile

compile:
	gcc -Wall -g *.c -lncurses -o csh
# Use -g to generate debug information
# Use -Wall to enable all warnings
