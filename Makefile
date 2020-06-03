all: compile

compile:
	gcc -Wall -g -rdynamic src/*.c -o csh
# Use -g to generate debug information
# Use -Wall to enable all warnings

clean_docs:
	rm -fr docs/html
	rm -fr docs/latex

docs: clean_docs
	doxygen docs/Doxyfile
