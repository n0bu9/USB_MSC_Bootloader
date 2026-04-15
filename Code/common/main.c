#include "main.h"
#include "ch554.h"
#include "system_ch55x.h"
#include "bootloader.h"
#include "internal_flash.h"
#include "bsp_led.h"

#ifdef PRINTF_DEBUG
    #include "uart.h"
#endif

uint8_t buff[10] = {0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
uint8_t read[10];

void main()
{
    system_init();
    EA = 1;
    Port_LED_Init();
#ifdef PRINTF_DEBUG
    uart1_init();
    delay_ms(10);
#endif
    // bootloader_main_task();
    if (write_code_flash(7900, &buff, 10) == 0xff)
        uart1_sendstr("ERR!\r\n", sizeof("ERR!\r\n"));
    else
        read_code_flash(7900, &read, 10);
        if (buff[0] == read[0]) LED_ON();
    uart1_sendstr(read, 10);

    while (1){
        // uart1_sendbyte('A');
        // LED_ON();
        // delay_ms(500);
        // LED_OFF();
        // delay_ms(500);
    }
}
