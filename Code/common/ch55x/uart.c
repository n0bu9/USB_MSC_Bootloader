#include "uart.h"
#include "system_ch55x.h"

/*******************************************************************************
* Function Name  : uart1_init()
* Description    : CH554串口1初始化 P1.6--RXD P1.7--TXD
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void uart1_init(void)
{
    U1SM0 = 0;                                                                   //UART1选择8位数据位
	U1SMOD = 1;                                                                  //快速模式
	U1REN = 1;                                                                   //使能接收
	SBAUD1 = 0 - FREQ_SYS/16/UART1_BUAD;
	U1TI = 0;
}

/*******************************************************************************
* Function Name  : uart1_sendbyte(uint8_t send_data)
* Description    : CH554UART1发送一个字节
* Input          : uint8_t send_data: 要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/
void uart1_sendbyte(uint8_t send_data)
{
	SBUF1 = send_data;                                                             //查询发送，中断方式可不用下面2条语句,但发送前需TI=0
	while(U1TI ==0);
	U1TI = 0;
}


void uart1_sendstr(uint8_t *buf, uint8_t len)
{
	while (len--)
	{
		uart1_sendbyte(*buf++);
	}
}
