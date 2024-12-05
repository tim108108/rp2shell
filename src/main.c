#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "lib/test.h"

#define CON_BUF_MAX_SIZE 256

/* ======== api ======== */
typedef struct point_console
{
    char buffer[CON_BUF_MAX_SIZE];
    char count;
    char argc;
    char *argv[CON_BUF_MAX_SIZE];
    char flag;
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
pt_console gconsole;
void app_code(char argc, char **argv)
{
    const uint8_t *flash_target_address = (const uint8_t *)(XIP_BASE);
    extern uint32_t __flash_binary_end;
    uint32_t code_end = (uint32_t)&__flash_binary_end;
    uint32_t flash_size = PICO_FLASH_SIZE_BYTES;
    uint32_t free_flash_space = flash_size - (code_end - XIP_BASE);

    printf("\n");
    printf("Reading code at flash:");
    for (int i = 0; i < (code_end - XIP_BASE) ; i++) {
        if (i % 16 == 0) {
            printf("\n[%05X]: ", i);
        }
        printf("%02X ", flash_target_address[i]);
    }
    printf("\n\n");
    printf("Flash total size: %u bytes (%u KB)\n", flash_size, flash_size / 1024);
    printf("Code end address: 0x%08x\n", code_end);
    printf("Free flash space: %u bytes (%u KB)\n", free_flash_space, free_flash_space / 1024);

    return;
}
void app_file(char argc, char **argv)
{
    extern uint32_t __flash_binary_end; 
    const uint8_t *flash_target_address = (const uint8_t *)(XIP_BASE);
    uint32_t code_end = (uint32_t)&__flash_binary_end;
    uint32_t flash_size = PICO_FLASH_SIZE_BYTES;
    uint32_t flash_target_offset = 0x40000;
    
    printf("\n");
    if (((uint32_t)&__flash_binary_end - XIP_BASE) > flash_target_offset) {
        printf("Error: Binary end address exceeds target offset.\n");
        return;
    }

    uint8_t random_data[FLASH_PAGE_SIZE];
    for (uint i = 0; i < FLASH_PAGE_SIZE; ++i){
        random_data[i] = rand() >> 16;
    }
    // printf("\nErasing target region...\n");
    // flash_range_erase(flash_target_offset, FLASH_SECTOR_SIZE);
    // printf("\nDone\n");
    // for (int i = flash_target_offset; i < flash_target_offset + FLASH_PAGE_SIZE; i++) {
    //     if (i % 16 == 0) {
    //         printf("\n[%05X]: ", i);
    //     }
    //     printf("%02X ", flash_target_address[i]);
    // }
    
    printf("\nProgramming target region...\n");
    flash_range_program(flash_target_offset, random_data, FLASH_PAGE_SIZE);
    printf("\nDone\n");
    for (int i = flash_target_offset; i < flash_target_offset + FLASH_PAGE_SIZE; i++) {
        if (i % 16 == 0) {
            printf("\n[%05X]: ", i);
        }
        printf("%02X ", flash_target_address[i]);
    }

    return;
}
void app_prt(char argc, char **argv)
{
    int i = 0;
    printf("\n");
    for (i = 0; i < strlen(argv[1]) + 4; i++){
        printf("=");
    }
    printf("\n= %s =\n",argv[1]);
    for (i = 0; i < strlen(argv[1]) + 4; i++){
        printf("=");
    }
    return;
}
void app_rnd(char argc, char **argv)
{
    rng_128_t rnd_num;
    get_rand_128 (&rnd_num);
    printf("\n0x%llX%llX", (unsigned long long)rnd_num.r[0], (unsigned long long)rnd_num.r[1]);
    return;
}
void app_top(char argc, char **argv)
{
    printf("\ncount:%d", gconsole.count);
    printf("\nargc:%d", gconsole.argc);
    printf("\nflag:%x", gconsole.flag);
    return;
}
void app_exit(char argc, char **argv)
{
    gconsole.flag = 0;
    printf("\n");
    printf("Function End~");
    return;
}

/* ======== cmd ======== */
console_cmd_arr console_cmd[] = 
{
    {"code", app_code},
    {"file", app_file},
    {"print", app_prt},
    {"top", app_top},
    {"rnd", app_rnd},
    {"exit", app_exit},
    {"\0", NULL}
};
void console_reset(pt_console *console)
{
    memset(console->buffer, 0, CON_BUF_MAX_SIZE);
    console->count = 0;
    console->argc = 0;
    console->argv[0] = 0;
    console->flag = 0xff;
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
{   // 根據console->buffer去尋找console_cmd[]是否有匹配，並將對應的cmd寫入console->buffer並printf()
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
    // if (!strcmp(console->buffer, "exit")){
    //     console->flag = 0;
    //     printf("\n");
    //     printf("Function End~");
    //     return;
    // }
    while (console_cmd[i].fp != NULL){
        if (!strcmp(console->argv[0], console_cmd[i].cmd)){
            console_cmd[i].fp(console->argc, console->argv);
            return;
        }
        i++;
    }
}
/* ======== main function ======== */
int main()
{
    stdio_init_all();

    // pt_console *pconsole = malloc(sizeof(pt_console));
    pt_console *pconsole = &gconsole;
    // sleep_ms(5000);
    pconsole->flag = 1; //set flag before loop
    while(pconsole->flag){
        console_reset(pconsole);        
        console_getchar(pconsole);
        console_getopt(pconsole);
        console_shell(pconsole);
    }
    return 0;
}