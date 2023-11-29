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
};

void console_reset(struct pt_console *console){
    memset(console->buffer, 0, CON_BUF_MAX_SIZE);
    console->count = 0;
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

void ls(char argc, char *argv){
    printf("ls\n");
}

int main()
{
    stdio_init_all();

    struct pt_console *pconsole = malloc(sizeof(struct pt_console));
    int i;

    sleep_ms(100);
    print_test_fun();
    
    while(1){

        console_reset(pconsole);        
        console_getchar(pconsole);
        if (!strcmp(pconsole->buffer, "exit")){
            break;
        }
        printf("\nrepeat:%s",pconsole->buffer);
    }
    printf("\nfunction_end");
    return 0;
}