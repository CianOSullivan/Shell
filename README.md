# Shell
![C/C++ CI](https://github.com/CianOSullivan/Shell/workflows/C/C++%20CI/badge.svg)
A Unix shell written in C.

# Build Instructions
Clone the repository:

```
git clone https://github.com/CianOSullivan/Shell
```

To run the shell:

```
cd Shell
make
./csh
```

The shell will then be executing! The shell can be run through a terminal emulator just as any other. For example, to use csh as the shell in urxvt, run the following:

```
urxvt -e /path/to/csh-executable
```

# Features
 - Ability to execute commands and navigate filesystem
 - Alias file which can be used to replace common commands
 - History file which records all user input

# TODO
 - Piping
 - Globbing
 - Escape Characters
 - Autocompletion using TAB
 - Command history retrieval using arrow keys (may require reimplementation using ncurses)
