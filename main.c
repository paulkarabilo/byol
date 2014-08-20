#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

#include <string.h>
#define MAX_BUF 2048
static char buffer[MAX_BUF]
char *readline(char *prompt) {
    fputs(prompt, stdout);
    fgets(buffer, MAX_BUF, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

void add_history(char *input);

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char **argv) {
    puts("PLisp v.0.0.0.1."); //Pablo Lisp lol
    puts("Press Ctrl-C to exit.");

    while (1) {
        char *input = readline("lispy> ");
        add_history(input);
        printf("No, you are %s\n", input);
        free(input);
    }
    return 0;
}
