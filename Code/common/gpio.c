#include "ch554.h"
#include "gpio.h"

/*******************************************************************************
* Function Name  : port1_config()
* Description    : 端口1配置
* Input          : mode 0 = 浮空输入，无上拉
                        1 = 推挽输入输出
                        2 = 开漏输入输出，无上拉
                        3 = 类51模式，开漏输入输出，有上拉，内部电路可以加速由低到高的电平爬升
                uint8_t pin	(0-7)
* Output         : None
* Return         : None
*******************************************************************************/
void port1_config(uint8_t mode,uint8_t pin)
{
    switch(mode){
    case 0:
        P1_MOD_OC = P1_MOD_OC & ~(1<<pin);
        P1_DIR_PU = P1_DIR_PU &	~(1<<pin);
        break;
    case 1:
        P1_MOD_OC = P1_MOD_OC & ~(1<<pin);
        P1_DIR_PU = P1_DIR_PU |	(1<<pin);
        break;
    case 2:
        P1_MOD_OC = P1_MOD_OC | (1<<pin);
        P1_DIR_PU = P1_DIR_PU &	~(1<<pin);
        break;
    case 3:
        P1_MOD_OC = P1_MOD_OC | (1<<pin);
        P1_DIR_PU = P1_DIR_PU |	(1<<pin);
        break;
    default:
        break;
    }
}

/*******************************************************************************
* Function Name  : port3_config()
* Description    : 端口3配置
* Input          : mode 0 = 浮空输入，无上拉
                        1 = 推挽输入输出
                        2 = 开漏输入输出，无上拉
                        3 = 类51模式，开漏输入输出，有上拉，内部电路可以加速由低到高的电平爬升
                uint8_t pin	(0-7)
* Output         : None
* Return         : None
*******************************************************************************/
void port3_config(uint8_t mode,uint8_t pin)
{
    switch(mode){
    case 0:
        P3_MOD_OC = P3_MOD_OC & ~(1<<pin);
        P3_DIR_PU = P3_DIR_PU &	~(1<<pin);
        break;
    case 1:
        P3_MOD_OC = P3_MOD_OC & ~(1<<pin);
        P3_DIR_PU = P3_DIR_PU |	(1<<pin);
        break;
    case 2:
        P3_MOD_OC = P3_MOD_OC | (1<<pin);
        P3_DIR_PU = P3_DIR_PU &	~(1<<pin);
        break;
    case 3:
        P3_MOD_OC = P3_MOD_OC | (1<<pin);
        P3_DIR_PU = P3_DIR_PU |	(1<<pin);
        break;
    default:
        break;
    }
}
