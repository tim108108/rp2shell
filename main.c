#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "lib/test.h"

int main()
{
    stdio_init_all();
    int i = 0;
    while(1){
        printf("hi\n");
        printf("%d\n",i);
        i++;
        sleep_ms(100);
    }
}