#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "lib/test.h"

#define CON_BUF_MAX_SIZE 256

/* ======== api ======== */
typedef struct point_console
{
    char buffer[CON_BUF_MAX_SIZE];
    char count;
    char argc;
    char *argv[CON_BUF_MAX_SIZE];
}pt_console;

typedef struct console_command_st
{
    char cmd[CON_BUF_MAX_SIZE];
    void (*fp)(char argc, char **argv);
}console_cmd_arr;
/* ======== header ======== */
void console_reset(pt_console *console);
void console_getchar(pt_console *console);
void console_tabcomplete(pt_console *console);
void console_getopt(pt_console *console);
void console_shell(pt_console *console);

/* ======== app ======== */
void ls_app(char argc, char **argv)
{
    printf("\napp:ls");
}
void cd_app(char argc, char **argv)
{
    printf("\napp:cd");
}
/* ======== cmd ======== */
console_cmd_arr console_cmd[] = 
{
    {"ls", ls_app},
    {"cd", cd_app},
    {"\0", NULL}
};
void console_reset(pt_console *console)
{
    memset(console->buffer, 0, CON_BUF_MAX_SIZE);
    console->count = 0;
    console->argc = 0;
    console->argv[0] = 0;
    printf("\n$~");
}
void console_getchar(pt_console *console)
{
    while(1) {
        int ch = getchar_timeout_us(0); // getchar can't read 'backspace', 'esc', etc special key......
        if (ch != PICO_ERROR_TIMEOUT){
            if (ch == '\r'){
                break;
                }
            else if(ch == '\t'){
                console_tabcomplete(console);
                }
            else if(console->count > CON_BUF_MAX_SIZE){
                printf("out of buffer!!\n");
                break;
                }
            else{
                putchar(ch);  
                console->buffer[console->count] = ch;
                console->count += 1;  
                } 
        }
    }
}
void console_tabcomplete(pt_console *console) 
{
    // 根據console->buffer去尋找console_cmd[]是否有匹配，並將對應的cmd寫入console->buffer並printf()
    int index = 0, i;
    char partial_cmd[CON_BUF_MAX_SIZE];

    if(console->count < 1){
        printf("\nCommand List:");
        while (console_cmd[index].fp != NULL){
            printf("\n%s", console_cmd[index].cmd);
            index++;
        }
        return;
    }
    memset(partial_cmd, 0, CON_BUF_MAX_SIZE);
    strcpy(partial_cmd, console->buffer);

    index = 0;
    while (console_cmd[index].fp != NULL){
        if(!strncmp(partial_cmd, console_cmd[index].cmd, strlen(console->buffer))){
            printf("%s",console_cmd[index].cmd+console->count);
            strcpy(console->buffer, console_cmd[index].cmd);
            console->count = strlen(console_cmd[index].cmd);
        }
        index++;
    }
    return;
}
void console_getopt(pt_console *console)
{
    int i;
    if (console->buffer[0] == '\0'){
        console->argc = 0;
        return;
    }
    console->argv[0] = &(console->buffer[0]);
    for (i = 0; i < console->count; i++){
        if (console->buffer[i] == ' ') {
            console->buffer[i] = '\0';
            console->argv[console->argc + 1] = &(console->buffer[i + 1]);
            console->argc += 1;
        } 
    }
    console->argc += 1;
}
void console_shell(pt_console *console)
{
    int i = 0;
    while (console_cmd[i].fp != NULL){
        // printf("\n%s",console_cmd[i].cmd);
        if (!strcmp(console->argv[0], console_cmd[i].cmd)){
            console_cmd[i].fp(console->argc, console->argv);
            return;
        }
        i++;
    }
}

int main()
{
    stdio_init_all();

    pt_console *pconsole = malloc(sizeof(pt_console));
    int i;

    // sleep_ms(5000);
    
    while(1){

        console_reset(pconsole);        
        console_getchar(pconsole);
        console_getopt(pconsole);
        console_shell(pconsole);
        if (!strcmp(pconsole->buffer, "exit")){
            break;
        }
        // printf("\nrepeat:%s",pconsole->buffer);
    }
    printf("\nfunction_end");
    return 0;
}