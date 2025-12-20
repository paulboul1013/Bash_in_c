#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

/* shell color code
\033:轉義字符
[:開始定義顏色
字體背景顏色:[40-47]
字體顏色:[30-37]
m:轉義結束
*/
#define RED "\033[0;31m"
#define RESET "\033[0m"

char *readline();
char **split_line(char *);
int dash_execute(char **);
int dash_exit(char**);

char *readline() {
    int buffsize=1024;
    int position=0;
    char *buffer=malloc(sizeof(char)*buffsize);
    int c;

    if(!buffer) {
        fprintf(stderr,"%sdash: Allocation error%s\n",RED,RESET);
        exit(EXIT_FAILURE);
    }

    while (1){
        c=getchar();
        if (c==EOF || c=='\n') {
            buffer[position]='\0';
            return buffer;
        }else{
            buffer[position++]=c;
        }

        if (position>=buffsize){
            buffsize<<=1;
            buffer=realloc(buffer,buffsize);

            if (!buffer) {
                fprintf(stderr,"dash: Allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
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