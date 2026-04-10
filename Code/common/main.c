#include "main.h"
#include "ch554.h"
#include "system_ch55x.h"
#include "bootloader.h"
#include "internal_flash.h"
#include "bsp_led.h"

#ifdef PRINTF_DEBUG
    #include "uart.h"
#endif

uint8_t buff[10] = {0x66, 0x00};

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
    if (write_code_flash(7900, &buff[0], 1) == 0xff)
        uart1_sendstr("ERR!\r\n", sizeof("ERR!\r\n"));
    else
        read_code_flash(7900, &buff[1], 1);
        if (buff[0] == buff[1]) LED_ON();
    uart1_sendbyte(buff[1]);

    while (1){
        // uart1_sendbyte('A');
        // LED_ON();
        // delay_ms(500);
        // LED_OFF();
        // delay_ms(500);
    }
}
