#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>   // pid_t
#include <unistd.h>      // fork, execvp
#include <sys/wait.h>    // waitpid, WUNTRACED
#include <readline/readline.h>
#include <readline/history.h>
#include <time.h>


#define TOK_DELIM " \t\r\n"
#define RED "\033[0;31m"
#define RESET "\e[0m"
#define GREEN "\033[0;32m"
#define BRIGHT_GREEN "\033[1;32m"

char *read_line();
char **split_line(char *line);
int dash_execute(char **args);
int dash_exit(char **args);
void loop();

// ASCII related built-in commands
int cmd_ascii_help(char **args);
int cmd_ascii_table(char **args);
int cmd_ascii_box(char **args);
int cmd_progress(char **args);
int cmd_spinner(char **args);
int cmd_matrix(char **args);

int TK_BUFF_SIZE=1024;

int dash_execute(char **args){
    pid_t cpid;
    int status;

    if (args==NULL || args[0]==NULL){
        return 1;   
    }
    
    if (strcmp(args[0],"exit")==0){
        return dash_exit(args);
    }

    // built-in ascii commands
    if (strcmp(args[0],"ascii-help")==0){
        return cmd_ascii_help(args);
    }
    if (strcmp(args[0],"ascii-table")==0){
        return cmd_ascii_table(args);
    }
    if (strcmp(args[0],"ascii-box")==0){
        return cmd_ascii_box(args);
    }
    if (strcmp(args[0],"progress")==0){
        return cmd_progress(args);
    }
    if (strcmp(args[0],"spinner")==0){
        return cmd_spinner(args);
    }
    if (strcmp(args[0],"matrix")==0){
        return cmd_matrix(args);
    }

    cpid=fork();

    if (cpid==0){
        if (execvp(args[0],args)<0){
            printf("dash: command not found: %s\n",args[0]);
            exit(EXIT_FAILURE);
        }
    }
    else if (cpid<0){
        printf(RED "Error forking%s\n",RESET);
    }
    else{
        waitpid(cpid,&status,WUNTRACED);
    }

    return 1;
}

int dash_exit(char **args){
    return 0;
}

int cmd_ascii_help(char **args){
    printf("ASCII commands:\n");
    printf("  ascii-help                 - 顯示此說明\n");
    printf("  ascii-table                - 列出可列印 ASCII 字元表 (32-126)\n");
    printf("  ascii-box [text...]        - 使用方框包住文字\n");
    printf("  progress [width] [ms]      - 顯示進度條動畫，預設 width=30 ms=1500\n");
    printf("  spinner [ms]               - 顯示轉輪動畫，預設 ms=1500\n");
    printf("  matrix [frames] [width]    - 顯示矩陣雨效果，預設 frames=40 width=80\n");
    return 1;
}

int cmd_ascii_table(char **args){
    int c;
    printf("+------+-----+-------+\n");
    printf("| DEC  | HEX | CHAR  |\n");
    printf("+------+-----+-------+\n");
    for (c = 32; c <= 126; c++){
        char ch = (char)c;
        if (ch=='\t' || ch=='\n' || ch=='\r') ch=' ';
        printf("| %4d | %3X |  %c    |\n", c, c, ch);
    }
    printf("+------+-----+-------+\n");
    return 1;
}

int cmd_ascii_box(char **args){
    int i;
    int total_len = 0;
    char *fallback = "Hello, ASCII!";
    if (args[1]==NULL){
        total_len = (int)strlen(fallback);
        printf("+");
        for (i=0;i<total_len+2;i++) printf("-");
        printf("+\n");
        printf("| %s |\n", fallback);
        printf("+");
        for (i=0;i<total_len+2;i++) printf("-");
        printf("+\n");
        return 1;
    }
    // compute length of joined args with spaces
    for (i=1; args[i]!=NULL; i++){
        total_len += (int)strlen(args[i]);
        if (args[i+1]!=NULL) total_len += 1; // space
    }
    printf("+");
    for (i=0;i<total_len+2;i++) printf("-");
    printf("+\n");
    printf("| ");
    for (i=1; args[i]!=NULL; i++){
        printf("%s", args[i]);
        if (args[i+1]!=NULL) printf(" ");
    }
    printf(" |\n");
    printf("+");
    for (i=0;i<total_len+2;i++) printf("-");
    printf("+\n");
    return 1;
}

int cmd_progress(char **args){
    int width = 30;
    int total_ms = 1500;
    int i;
    if (args[1]!=NULL){
        int w = atoi(args[1]);
        if (w>0 && w<300) width = w;
    }
    if (args[2]!=NULL){
        int ms = atoi(args[2]);
        if (ms>50 && ms<60000) total_ms = ms;
    }
    int steps = 100;
    int delay_us = (total_ms*1000)/steps;
    if (delay_us<10000) delay_us = 10000; // 至少 10ms
    for (i=0;i<=steps;i++){
        int filled = (i*width)/steps;
        int j;
        printf("\r[");
        for (j=0;j<filled;j++) printf("#");
        for (j=filled;j<width;j++) printf(" ");
        printf("] %3d%%", i);
        fflush(stdout);
        usleep(delay_us);
    }
    printf("\n");
    return 1;
}

int cmd_spinner(char **args){
    const char frames[] = "|/-\\"; // 注意轉義反斜線
    int duration_ms = 1500;
    if (args[1]!=NULL){
        int v = atoi(args[1]);
        if (v>50 && v<60000) duration_ms = v;
    }
    int delay_us = 80000; // 80ms
    int iterations = (duration_ms*1000)/delay_us;
    int i;
    for (i=0;i<iterations;i++){
        char f = frames[i % 4];
        printf("\r[%c] spinning...", f);
        fflush(stdout);
        usleep(delay_us);
    }
    printf("\rDone            \n");
    return 1;
}

int cmd_matrix(char **args){
    static int seeded = 0;
    if (!seeded){
        seeded = 1;
        srand((unsigned int)(time(NULL) ^ (unsigned long)(args)));
    }
    int frames = 40;
    int width = 80;
    if (args[1]!=NULL){
        int f = atoi(args[1]);
        if (f>1 && f<2000) frames = f;
    }
    if (args[2]!=NULL){
        int w = atoi(args[2]);
        if (w>10 && w<300) width = w;
    }
    const char *glyphs = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int glyphs_len = (int)strlen(glyphs);
    int f,i;
    for (f=0; f<frames; f++){
        printf(GREEN);
        for (i=0;i<width;i++){
            int r = rand()%glyphs_len;
            int bright = rand()%20==0; // 少量高亮
            if (bright) printf(BRIGHT_GREEN);
            putchar(glyphs[r]);
            if (bright) printf(GREEN);
        }
        printf(RESET"\n");
        fflush(stdout);
        usleep(50000); // 50ms
    }
    return 1;
}

char **split_line(char *line) {
    int buffsize=1024,position=0;
    char **tokens=malloc(sizeof(char*)*buffsize);
    char *token;

    if (!tokens){
        fprintf(stderr,"%s dash: Allocation error%s\n",RED,RESET);
        exit(EXIT_FAILURE);
    }

    token=strtok(line,TOK_DELIM);
    while (token!=NULL){
        tokens[position++]=token;

        if (position>=buffsize){
            buffsize+=TK_BUFF_SIZE;
            tokens=realloc(tokens,sizeof(char*)*buffsize);

            if (!tokens){
                fprintf(stderr,"%s dash: Allocation error%s\n",RED,RESET);
                exit(EXIT_FAILURE);
            }

        }
        
        token=strtok(NULL,TOK_DELIM);
    }

    tokens[position]=NULL;

    return tokens;
}

//有歷史功能德read_line
char *read_line() {
	char *line = readline("> ");
	if (line == NULL) {
		return NULL; // EOF / Ctrl-D
	}
	if (*line) {
		add_history(line);
	}
	return line;
}

// char *read_line() {
//     int buffsize= 1024;
//     int position =0;
//     char *buffer=malloc(sizeof(char)*buffsize);
//     int c;

//     if (!buffer){
//         fprintf(stderr,"%s dash: Allocation error%s\n",RED,RESET);
//         exit(EXIT_FAILURE);
//     }

//     while (1){
//         c=getchar();
//         if (c==EOF || c=='\n'|| c=='\r'){
//             buffer[position]='\0';
//             return buffer;
//         }
//         else{
//             buffer[position]=c;
//         }

//         position++;

//         if (position>=buffsize){
//             buffsize+=1024;
//             buffer=realloc(buffer,buffsize);

//             if (!buffer){
//                 fprintf(stderr,"dash: Allocation error\n");
//                 exit(EXIT_FAILURE);
//             }
//         }
//     }
// }

void loop() {
    char *line=NULL;
    char **args=NULL;
    int status=1;

    do{
        // printf("> ");
        line=read_line();
        if (line==NULL){
            continue;
        }
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