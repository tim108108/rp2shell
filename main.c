#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "lib/test.h"

#define CON_BUF_MAX_SIZE 256
struct pt_console{
    char buffer[CON_BUF_MAX_SIZE];
    char count;
    char argc;
    char *argv[CON_BUF_MAX_SIZE];
};

void console_reset(struct pt_console *console){
    memset(console->buffer, 0, CON_BUF_MAX_SIZE);
    console->count = 0;
    console->argc = 0;
    printf("\n$~");
}
void console_getchar(struct pt_console *console){
    while(1) {
        int ch = getchar_timeout_us(0); // getchar can't read 'backspace', 'esc', etc special key......
        if (ch != PICO_ERROR_TIMEOUT){
            if (ch == '\r'){
                // printf("\ncount:%d",*count);
                // putchar('\r');
                // putchar('\n');
                break;
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
void console_getopt(struct pt_console *console){
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
/* app */
void ls(){
    printf("\napp:ls");
}
void cd(){
    printf("\napp:cd");
}
void app_handler(struct pt_console *console){
    printf("\n============\napp handler:");
    int i = 0;
    if (strcmp(console->argv[0], "ls") == 0){
        ls();
    }else if (strcmp(console->argv[0], "cd") == 0){
        cd();
    }else {
        printf("\nnot func");
    }
    printf("\napp end.\n============");
}
int main()
{
    stdio_init_all();

    struct pt_console *pconsole = malloc(sizeof(struct pt_console));
    int i;

    // sleep_ms(5000);
    
    while(1){

        console_reset(pconsole);        
        console_getchar(pconsole);
        console_getopt(pconsole);
        app_handler(pconsole);
        if (!strcmp(pconsole->buffer, "exit")){
            break;
        }
        // printf("\nrepeat:%s",pconsole->buffer);
    }
    printf("\nfunction_end");
    return 0;
}