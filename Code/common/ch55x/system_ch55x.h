#ifndef __SYSTEM_CH55X_H__
#define __SYSTEM_CH55X_H__

#include "stdint.h"

#define	 FREQ_SYS	   24000000	         //系统主频12MHz
#define  OSC_EN_XT     0                 //外部晶振使能，默认开启内部晶振

void system_init(void);
void delay_us(uint16_t n);
void delay_ms(uint16_t n);

#endif
