#ifndef __UART_H__
#define __UART_H__

#include "stdint.h"

#define  UART1_BUAD   57600

void uart1_init(void);
void uart1_sendbyte(uint8_t send_data);
void uart1_sendstr(uint8_t *buf, uint8_t len);

#endif
