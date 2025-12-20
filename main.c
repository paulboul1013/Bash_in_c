#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

char *readline();
char **split_line(char *);
int dash_execute(char **);
int dash_exit(char**);

char *readline() {
    return NULL;
}

char **split_line(char *line) {
    return NULL;
}

int dash_execute(char **args) {
    return 0;
}

int dash_exit(char **args) {
    return 0;
}

void loop() {
    char *line;
    char **args;
    int status=1;
    
    do{
        printf("> ");
        line=readline();
        args=split_line(line);
        status=dash_execute(args);
        free(line);
        free(args);
    }while(status);
}

int main() {
    loop();
    
    return 0;
}