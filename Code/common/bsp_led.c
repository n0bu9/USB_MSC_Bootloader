#include "gpio.h"
#include "bsp_led.h"


void Port_LED_Init(void)
{
    // set LED pin as output
    port3_config(1, 2);  // init gpio pin
    LED_OFF();  // turn off LED
}

void Port_LED_DeInit(void)
{
    // set LED pin as input (high impedance)
    port3_config(0, 2);  // deinit gpio pin
}
