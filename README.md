# Shell
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

# TODO
 - Piping
 - Compound arguments e.g:
   ```
   ls -a -l    # This works
   ls -al      # This doesn't work
   ```
 - Globbing
 - Escape Characters
 - Autocompletion using TAB
 - Command history retrieval using arrow keys (may require reimplementation using ncurses)
