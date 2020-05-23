#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

char* readline() {
    int bufSize = BUFSIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * bufSize);
    char c;

    while (true) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position++] = c;
        }

        if (position >= bufSize) {
            bufSize += BUFSIZE;
            buffer = realloc(buffer, bufSize);
        }
    }

    return buffer;
}

void loop() {
    bool running = true;
    char* line;
    while (running) {
        printf("> ");
        line = readline();
        printf("String entered: %s\n", line);
    }

}

int main(int argc, char **argv) {
    loop();
    return 0;

}
