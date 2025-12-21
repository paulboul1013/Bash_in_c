#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

/* shell color code
\033:轉義字符
[:開始定義顏色
字體背景顏色:[40-47]
字體顏色:[30-37]
m:轉義結束
*/
#define RED "\033[0;31m"
#define RESET "\033[0m"

static char **history_list=NULL;//history save list
static int *history_count=NULL;//history count

#define TOK_DELIM " \t\r\n"

#define TK_BUFF_SIZE 64

#define SHM_NAME "/my_shared_mem"




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
        if (c==EOF || c=='\n' || c=='\t') {
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
    int buffsize=TK_BUFF_SIZE,position=0;
    char **tokens=malloc(sizeof(char*)*buffsize);//define two-dimensional array
    char *token;

    if (!tokens){
        fprintf(stderr,"%sdash: Allocation error%s\n",RED,RESET);
        exit(EXIT_FAILURE);
    }

    token=strtok(line,TOK_DELIM);//split the line into token
    while (token!=NULL){
        tokens[position++]=token;

        if (position>=buffsize){
            buffsize+=TK_BUFF_SIZE;
            tokens=realloc(tokens,sizeof(char*)*buffsize);

            if (!tokens){
                fprintf(stderr,"%sdash: Allocation error%s\n",RED,RESET);
                exit(EXIT_FAILURE);
            }
        }

        token=strtok(NULL,TOK_DELIM);//get the next token
    }

    tokens[position]='\0';


    return tokens;
}

int dash_exit(char **args) {
    (void)args;
    return 0;
}


//define the shell command
int paul(char **args){
    (void)args;
    printf("Paul is a project author\n");
    exit(EXIT_SUCCESS);
    return 1;
}

int history(char **args){
    (void)args;
    printf("History %d\n",(*history_count));
    for(int i=0;i<(*history_count);i++){
        printf("%d. %s\n",i+1,history_list[i]);
    }
    exit(EXIT_SUCCESS);
    return 1;
}

int dash_execute(char **args) {
    pid_t cpid;
    int status;

    int (*builtins[])(char **) = {
        &paul,
        &history,
    };
    
    if (strncmp(args[0],"exit",4)==0){
        return dash_exit(args);
    }

    cpid=fork();

    if (cpid==0) {
        //execute the self defined shell command
        if (strncmp(args[0],"paul",4)==0){
            history_list[(*history_count)]="paul";
            (*history_count)++;
            builtins[0](args);

        }
        else if (strncmp(args[0],"history",7)==0){
            history_list[(*history_count)]="history";
            (*history_count)++;
            builtins[1](args);
        }
        //execute original shell command
        else if (execvp(args[0],args)>0){

            int pass=0xDEAD;
        }else{
            printf("dash: command not found: %s\n",args[0]);
            exit(EXIT_FAILURE);
        }

    } else if (cpid < 0){
        printf(RED "Error forking" RESET "\n");

    }else{
        waitpid(cpid,&status,WUNTRACED);
    }
    

    return 1;
}



void loop() {
    char *line=malloc(sizeof(char)*1024);
    char **args=malloc(sizeof(char*)*1024);
    int status=1;
    int shm_fd;

    // Create or open shared memory object
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    // Set size of shared memory
    if (ftruncate(shm_fd, sizeof(int) + sizeof(char*)*100) == -1) {
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    // Map shared memory into process address space
    void *shared_mem = mmap(NULL, sizeof(int) + sizeof(char*)*100, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    // Set pointers to different parts of shared memory
    history_count = (int*)shared_mem;
    history_list = (char**)((char*)shared_mem + sizeof(int));

    // Initialize shared variable in parent
    (*history_count) = 0;
    memset(history_list, 0, sizeof(char*)*100);


    do{
        printf("> ");
        line=readline();
        if (line==NULL || strncmp(line,"\0",1)==0){
            printf("\r");
            continue;
        }
        args=split_line(line);
        status=dash_execute(args);
        // printf("history_count: %d\n",(*history_count));

        free(line);
        free(args);
    }while(status);
}

int main() {
    loop();
    
    return 0;
}