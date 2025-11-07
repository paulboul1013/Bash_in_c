#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>   // pid_t
#include <sys/stat.h>    // stat, utime
#include <utime.h>       // utime
#include <unistd.h>      // fork, execvp, unlink
#include <sys/wait.h>    // waitpid, WUNTRACED
#include <readline/readline.h>
#include <readline/history.h>
#include <time.h>
#include <errno.h>       // errno


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
int has_pipe(char **args);
int execute_pipe(char **args);

// Built-in commands
int cmd_cd(char **args);
int cmd_help(char **args);
int cmd_cat(char **args);
int cmd_touch(char **args);
int cmd_rm(char **args);

// ASCII related built-in commands
int cmd_ascii_help(char **args);
int cmd_ascii_table(char **args);
int cmd_ascii_box(char **args);
int cmd_progress(char **args);
int cmd_spinner(char **args);
int cmd_matrix(char **args);
int cmd_clock(char **args);
int cmd_weather(char **args);

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

    // built-in commands
    if (strcmp(args[0],"cd")==0){
        return cmd_cd(args);
    }
    if (strcmp(args[0],"help")==0){
        return cmd_help(args);
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
    if (strcmp(args[0],"clock")==0){
        return cmd_clock(args);
    }
    if (strcmp(args[0],"weather")==0){
        return cmd_weather(args);
    }
    if (strcmp(args[0],"cat")==0){
        return cmd_cat(args);
    }
    if (strcmp(args[0],"touch")==0){
        return cmd_touch(args);
    }
    if (strcmp(args[0],"rm")==0){
        return cmd_rm(args);
    }

    // Check for pipe
    if (has_pipe(args)){
        return execute_pipe(args);
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

// Built-in cd command
int cmd_cd(char **args){
    char cwd[1024];
    static char prev_dir[1024] = {0};  // Store previous directory
    
    // No arguments or "cd ~" -> go to HOME
    if (args[1] == NULL || strcmp(args[1], "~") == 0){
        char *home = getenv("HOME");
        if (home == NULL){
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
        // Save current directory before changing
        if (getcwd(cwd, sizeof(cwd)) != NULL){
            strncpy(prev_dir, cwd, sizeof(prev_dir) - 1);
        }
        if (chdir(home) != 0){
            perror("cd");
            return 1;
        }
        return 1;
    }
    
    // cd - -> go to previous directory
    if (strcmp(args[1], "-") == 0){
        if (prev_dir[0] == '\0'){
            fprintf(stderr, "cd: OLDPWD not set\n");
            return 1;
        }
        // Save current directory
        char temp[1024];
        if (getcwd(temp, sizeof(temp)) == NULL){
            perror("cd");
            return 1;
        }
        // Change to previous directory
        if (chdir(prev_dir) != 0){
            perror("cd");
            return 1;
        }
        // Update previous directory
        strncpy(prev_dir, temp, sizeof(prev_dir) - 1);
        printf("%s\n", prev_dir);  // Print the directory we switched to
        return 1;
    }
    
    // cd <path> -> go to specified directory
    // Save current directory before changing
    if (getcwd(cwd, sizeof(cwd)) != NULL){
        strncpy(prev_dir, cwd, sizeof(prev_dir) - 1);
    }
    
    if (chdir(args[1]) != 0){
        perror("cd");
        return 1;
    }
    
    return 1;
}

// Built-in help command
int cmd_help(char **args){
    printf("Bash_in_c - 簡易 Shell\n\n");
    printf("內建指令:\n");
    printf("  help                       - 顯示此說明訊息\n");
    printf("  exit                       - 退出 shell\n");
    printf("  cd [dir]                   - 切換目錄\n");
    printf("                               cd          -> 切換到 HOME 目錄\n");
    printf("                               cd ~        -> 切換到 HOME 目錄\n");
    printf("                               cd -        -> 切換到上一個目錄\n");
    printf("                               cd <path>   -> 切換到指定目錄\n");
    printf("  ascii-help                 - 顯示 ASCII 藝術指令說明\n");
    printf("\nASCII 藝術指令:\n");
    printf("  ascii-table                - 列出可列印 ASCII 字元表\n");
    printf("  ascii-box [text...]        - 使用方框包住文字\n");
    printf("  progress [width] [ms]      - 顯示進度條動畫\n");
    printf("  spinner [ms]               - 顯示轉輪動畫\n");
    printf("  matrix [frames] [width]    - 顯示矩陣雨效果\n");
    printf("  clock [duration]           - 顯示 ASCII 藝術時鐘\n");
    printf("  weather [city]             - 顯示天氣資訊\n");
    printf("\n檔案操作指令:\n");
    printf("  cat [file...]              - 顯示檔案內容\n");
    printf("  touch [file...]            - 創建檔案或更新時間戳\n");
    printf("  rm [file...]               - 刪除檔案\n");
    printf("\n其他:\n");
    printf("  任何系統指令 (如 ls, pwd, echo 等)\n");
    printf("\n快捷鍵:\n");
    printf("  ↑/↓                        - 瀏覽命令歷史\n");
    printf("  Ctrl+C                     - 中斷當前指令\n");
    printf("  Ctrl+D                     - 退出 shell\n");
    return 1;
}

int cmd_ascii_help(char **args){
    printf("ASCII commands:\n");
    printf("  ascii-help                 - 顯示此說明\n");
    printf("  ascii-table                - 列出可列印 ASCII 字元表 (32-126)\n");
    printf("  ascii-box [text...]        - 使用方框包住文字\n");
    printf("  progress [width] [ms]      - 顯示進度條動畫，預設 width=30 ms=1500\n");
    printf("  spinner [ms]               - 顯示轉輪動畫，預設 ms=1500\n");
    printf("  matrix [frames] [width]    - 顯示矩陣雨效果，預設 frames=40 width=80\n");
    printf("  clock [duration]           - 顯示 ASCII 藝術時鐘，預設持續 10 秒\n");
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

// ASCII art clock command
int cmd_clock(char **args){
    // ASCII art digits (5 rows each)
    const char *digits[10][5] = {
        // 0
        {" ███ ", "█   █", "█   █", "█   █", " ███ "},
        // 1
        {"  █  ", " ██  ", "  █  ", "  █  ", " ███ "},
        // 2
        {" ███ ", "█   █", "   █ ", "  █  ", "█████"},
        // 3
        {" ███ ", "█   █", "  ██ ", "█   █", " ███ "},
        // 4
        {"█   █", "█   █", "█████", "    █", "    █"},
        // 5
        {"█████", "█    ", "████ ", "    █", "████ "},
        // 6
        {" ███ ", "█    ", "████ ", "█   █", " ███ "},
        // 7
        {"█████", "    █", "   █ ", "  █  ", " █   "},
        // 8
        {" ███ ", "█   █", " ███ ", "█   █", " ███ "},
        // 9
        {" ███ ", "█   █", " ████", "    █", " ███ "}
    };
    
    // Colon separator (5 rows)
    const char *colon[5] = {"  ", "█ ", "  ", "█ ", "  "};
    
    int duration = 10; // default 10 seconds
    if (args[1]!=NULL){
        int d = atoi(args[1]);
        if (d>0 && d<3600) duration = d;
    }
    
    time_t start_time = time(NULL);
    time_t current_time;
    
    printf("\033[?25l"); // Hide cursor
    
    while (1){
        current_time = time(NULL);
        if (difftime(current_time, start_time) >= duration){
            break;
        }
        
        struct tm *tm_info = localtime(&current_time);
        int hour = tm_info->tm_hour;
        int min = tm_info->tm_min;
        int sec = tm_info->tm_sec;
        
        // Get individual digits
        int h1 = hour / 10;
        int h2 = hour % 10;
        int m1 = min / 10;
        int m2 = min % 10;
        int s1 = sec / 10;
        int s2 = sec % 10;
        
        // Clear screen and move to top
        printf("\033[2J\033[H");
        
        // Print clock title
        printf("\n   ╔═══════════════════════════════════════════════════╗\n");
        printf("   ║            ASCII 藝術時鐘 ⏰                      ║\n");
        printf("   ╚═══════════════════════════════════════════════════╝\n\n");
        
        // Print each row of the digits
        int row;
        for (row=0; row<5; row++){
            printf("        ");
            printf("%s", digits[h1][row]);
            printf("%s", digits[h2][row]);
            printf("%s", colon[row]);
            printf("%s", digits[m1][row]);
            printf("%s", digits[m2][row]);
            printf("%s", colon[row]);
            printf("%s", digits[s1][row]);
            printf("%s", digits[s2][row]);
            printf("\n");
        }
        
        // Print date
        printf("\n           %04d/%02d/%02d (%s)\n", 
               tm_info->tm_year + 1900, 
               tm_info->tm_mon + 1, 
               tm_info->tm_mday,
               (tm_info->tm_wday==0)?"日":(tm_info->tm_wday==1)?"一":
               (tm_info->tm_wday==2)?"二":(tm_info->tm_wday==3)?"三":
               (tm_info->tm_wday==4)?"四":(tm_info->tm_wday==5)?"五":"六");
        
        printf("\n        按 Ctrl+C 停止顯示...\n");
        
        fflush(stdout);
        usleep(500000); // Update every 0.5 seconds
    }
    
    printf("\033[?25h"); // Show cursor
    printf("\n");
    return 1;
}

int cmd_weather(char **args) {
    // 檢查 curl 是否可用
    if (system("command -v curl > /dev/null 2>&1") != 0) {
        printf(RED "錯誤: 找不到 curl 命令。請先安裝 curl%s\n", RESET);
        printf("    Ubuntu/Debian: sudo apt-get install curl\n");
        printf("    CentOS/RHEL:   sudo yum install curl\n");
        return 1;
    }
    
    // 構建 URL
    char url[512];
    char city[256] = "";
    
    if (args[1] != NULL) {
        // 有指定城市
        strncpy(city, args[1], sizeof(city) - 1);
        city[sizeof(city) - 1] = '\0';
        
        // URL 編碼城市名稱（簡單處理，將空格替換為+）
        char encoded_city[256];
        int j = 0;
        for (int i = 0; city[i] != '\0' && j < sizeof(encoded_city) - 1; i++) {
            if (city[i] == ' ') {
                encoded_city[j++] = '+';
            } else {
                encoded_city[j++] = city[i];
            }
        }
        encoded_city[j] = '\0';
        
        snprintf(url, sizeof(url), "https://wttr.in/%s?format=3", encoded_city);
    } else {
        // 沒有指定城市，使用當前位置
        snprintf(url, sizeof(url), "https://wttr.in/?format=3");
    }
    
    printf(GREEN "正在查詢天氣資訊...\n" RESET);
    if (args[1] != NULL) {
        printf("城市: %s\n\n", city);
    } else {
        printf("位置: 自動偵測\n\n");
    }
    
    // 使用 curl 獲取天氣資訊
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "curl -s '%s'", url);
    
    int ret = system(cmd);
    
    if (ret != 0) {
        printf(RED "無法獲取天氣資訊。請檢查網路連線或稍後再試。%s\n", RESET);
        return 1;
    }
    
    printf("\n");
    printf("提示: 使用 'curl wttr.in/%s' 查看更詳細的天氣資訊\n", 
           args[1] != NULL ? city : "");
    
    return 1;
}

// Built-in cat command - 顯示檔案內容
int cmd_cat(char **args) {
    if (args[1] == NULL) {
        // 沒有指定檔案，從標準輸入讀取
        char buffer[4096];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
            fwrite(buffer, 1, bytes_read, stdout);
        }
        return 1;
    }
    
    int i;
    int error_occurred = 0;
    
    for (i = 1; args[i] != NULL; i++) {
        FILE *file = fopen(args[i], "r");
        if (file == NULL) {
            fprintf(stderr, "cat: %s: %s\n", args[i], strerror(errno));
            error_occurred = 1;
            continue;
        }
        
        char buffer[4096];
        size_t bytes_read;
        
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            fwrite(buffer, 1, bytes_read, stdout);
        }
        
        if (ferror(file)) {
            fprintf(stderr, "cat: %s: 讀取錯誤\n", args[i]);
            error_occurred = 1;
        }
        
        fclose(file);
    }
    
    return error_occurred ? 1 : 1;
}

// Built-in touch command - 創建檔案或更新時間戳
int cmd_touch(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "touch: 缺少檔案名稱\n");
        return 1;
    }
    
    int i;
    int error_occurred = 0;
    
    for (i = 1; args[i] != NULL; i++) {
        FILE *file = fopen(args[i], "a");
        if (file == NULL) {
            fprintf(stderr, "touch: %s: %s\n", args[i], strerror(errno));
            error_occurred = 1;
            continue;
        }
        fclose(file);
        
        // 更新時間戳
        struct utimbuf times;
        times.actime = time(NULL);
        times.modtime = time(NULL);
        
        if (utime(args[i], &times) != 0) {
            // 如果檔案不存在，utime 會失敗，但我們已經創建了檔案
            // 所以這裡的錯誤可以忽略（檔案已創建）
            if (errno != ENOENT) {
                fprintf(stderr, "touch: %s: 無法更新時間戳: %s\n", args[i], strerror(errno));
                error_occurred = 1;
            }
        }
    }
    
    return error_occurred ? 1 : 1;
}

// Built-in rm command - 刪除檔案
int cmd_rm(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "rm: 缺少檔案名稱\n");
        return 1;
    }
    
    int i;
    int error_occurred = 0;
    
    for (i = 1; args[i] != NULL; i++) {
        // 檢查是否為目錄
        struct stat path_stat;
        if (stat(args[i], &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                fprintf(stderr, "rm: %s: 是一個目錄（不支援刪除目錄）\n", args[i]);
                error_occurred = 1;
                continue;
            }
        }
        
        if (unlink(args[i]) != 0) {
            fprintf(stderr, "rm: %s: %s\n", args[i], strerror(errno));
            error_occurred = 1;
        }
    }
    
    return error_occurred ? 1 : 1;
}

char **split_line(char *line) {
    int buffsize=1024,position=0;
    char **tokens=malloc(sizeof(char*)*buffsize);
    char *token_start = NULL;
    int in_quotes = 0;
    int i = 0;
    int token_len = 0;

    if (!tokens){
        fprintf(stderr,"%s dash: Allocation error%s\n",RED,RESET);
        exit(EXIT_FAILURE);
    }

    // Skip leading whitespace
    while (line[i] && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\n')){
        i++;
    }

    token_start = &line[i];
    
    while (line[i]) {
        if (line[i] == '"' && (i == 0 || line[i-1] != '\\')){
            // Toggle quote state and remove the quote
            in_quotes = !in_quotes;
            // Shift characters left to remove the quote
            int j;
            for (j = i; line[j]; j++){
                line[j] = line[j+1];
            }
            continue;  // Don't increment i, check the same position again
        }
        
        if (!in_quotes && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\n')){
            // End of token
            if (token_len > 0){
                line[i] = '\0';
                tokens[position++] = token_start;
                
                if (position>=buffsize){
                    buffsize+=TK_BUFF_SIZE;
                    tokens=realloc(tokens,sizeof(char*)*buffsize);
                    if (!tokens){
                        fprintf(stderr,"%s dash: Allocation error%s\n",RED,RESET);
                        exit(EXIT_FAILURE);
                    }
                }
                token_len = 0;
            }
            
            // Skip whitespace
            i++;
            while (line[i] && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\n')){
                i++;
            }
            token_start = &line[i];
            continue;
        }
        
        token_len++;
        i++;
    }
    
    // Add last token if exists
    if (token_len > 0){
        tokens[position++] = token_start;
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

// Check if command contains pipe
int has_pipe(char **args){
    int i = 0;
    while (args[i] != NULL){
        if (strcmp(args[i], "|") == 0){
            return 1;
        }
        i++;
    }
    return 0;
}

// Execute piped commands
int execute_pipe(char **args){
    int i, j;
    int pipe_count = 0;
    
    // Count number of pipes
    for (i = 0; args[i] != NULL; i++){
        if (strcmp(args[i], "|") == 0){
            pipe_count++;
        }
    }
    
    if (pipe_count == 0){
        return 1;
    }
    
    // Split commands by pipe
    char ***commands = malloc(sizeof(char**) * (pipe_count + 1));
    if (!commands){
        fprintf(stderr, "dash: allocation error\n");
        return 1;
    }
    
    int cmd_idx = 0;
    int arg_start = 0;
    
    for (i = 0; i <= pipe_count; i++){
        commands[i] = malloc(sizeof(char*) * 1024);
        if (!commands[i]){
            fprintf(stderr, "dash: allocation error\n");
            return 1;
        }
    }
    
    // Parse commands
    j = 0;
    for (i = 0; args[i] != NULL; i++){
        if (strcmp(args[i], "|") == 0){
            commands[cmd_idx][j] = NULL;
            cmd_idx++;
            j = 0;
        } else {
            commands[cmd_idx][j++] = args[i];
        }
    }
    commands[cmd_idx][j] = NULL;
    
    // Create pipes
    int (*pipes)[2] = malloc(sizeof(int[2]) * pipe_count);
    if (!pipes){
        fprintf(stderr, "dash: allocation error\n");
        return 1;
    }
    
    for (i = 0; i < pipe_count; i++){
        if (pipe(pipes[i]) < 0){
            perror("pipe");
            return 1;
        }
    }
    
    // Execute commands
    pid_t pid;
    int num_cmds = pipe_count + 1;
    
    for (i = 0; i < num_cmds; i++){
        pid = fork();
        
        if (pid < 0){
            perror("fork");
            return 1;
        }
        
        if (pid == 0){
            // Child process
            
            // If not first command, get input from previous pipe
            if (i > 0){
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            // If not last command, output to next pipe
            if (i < num_cmds - 1){
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // Close all pipe file descriptors
            for (j = 0; j < pipe_count; j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Execute command
            if (execvp(commands[i][0], commands[i]) < 0){
                printf("dash: command not found: %s\n", commands[i][0]);
                exit(EXIT_FAILURE);
            }
        }
    }
    
    // Parent process: close all pipes
    for (i = 0; i < pipe_count; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all children
    for (i = 0; i < num_cmds; i++){
        wait(NULL);
    }
    
    // Free memory
    for (i = 0; i <= pipe_count; i++){
        free(commands[i]);
    }
    free(commands);
    free(pipes);
    
    return 1;
}

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