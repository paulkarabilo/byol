#include <stdio.h>

#define BUF_SIZE 2048
static char buffer[BUF_SIZE];

int main(int argc, char **argv) {
    puts("PLisp v.0.0.0.1."); //Pablo Lisp lol
    puts("Press Ctrl-C to exit.");

    while (1) {
        fputs("lispy> ", stdout);
        fgets(buffer, BUF_SIZE, stdin);
        printf("No, you are %s\n", buffer);
    }
    return 0;
}
