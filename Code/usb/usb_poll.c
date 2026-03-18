#include "usb_poll.h"

/*******************************************************************************
* Function Name  : enp1_in_evt()
* Description    : USB设备模式端点1的中断上传
* Input          : data[]: 数据
*                  len: 数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void enp1_in_evt(uint8_t usb_data[], uint8_t len)
{
    memcpy(Ep1Buffer + THIS_ENDP1_SIZE, usb_data, len);                                           //加载上传数据
    UEP1_T_LEN = len;                                                           //上传数据长度
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                   //有数据时上传数据并应答ACK
}

