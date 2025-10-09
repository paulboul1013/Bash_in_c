#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>   // pid_t
#include <unistd.h>      // fork, execvp
#include <sys/wait.h>    // waitpid, WUNTRACED
#include <readline/readline.h>
#include <readline/history.h>


#define TOK_DELIM " \t\r\n"
#define RED "\033[0;31m"
#define RESET "\e[0m"

char *read_line();
char **split_line(char *line);
int dash_execute(char **args);
int dash_exit(char **args);
void loop();

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