#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "ch554.h"

sbit LED_PIN = P3^2;  // define LED pin

#define LED_ON() do{ \
                    LED_PIN = 1; \
                } while (0);

#define LED_OFF() do{ \
                    LED_PIN = 0; \
                } while (0);

void Port_LED_Init(void);

#endif  // __BSP_LED_H__
