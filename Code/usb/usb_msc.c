#include "usb_msc.h"

#define THIS_ENDP0_SIZE		32UL
#define THIS_ENDP1_SIZE		32UL

static uint8_t xdata Ep0Buffer[MIN(THIS_ENDP0_SIZE+2, 64)] _at_ 0x0000;    //端点0 OUT&IN缓冲区，必须是偶地址
static uint8_t xdata Ep1Buffer[2*MIN(THIS_ENDP1_SIZE+2, 64)] _at_ MIN(THIS_ENDP0_SIZE+2, 64);  //端点1 OUT&IN缓冲区，必须是偶地址

uint8_t device_descriptor[] = {
    0x12,  // bLength
    0x01,  // bDescriptorType
    0x10, 0x01,  // usb1.1
    0x08, // bDeviceClass-mass storage
    0x00, // bDeviceSubClass
    0x00, // bDeviceProtocol
    0x32, // bMaxPacketSize0
    0x3d, 0x41, // idVendor
    0x07, 0x21, // idProduct
    0x00, 0x01, // bcdDevice
    0x00, // iManufacturer
    0x00, // iProduct
    0x00, // iSerialNumber
    0x01  // bNumConfigurations
}

uint8_t configuration_descriptor[] = {
    // configuration descriptor
    0x09, // bLength
    0x02, // bDescriptorType
    0x20, 0x00, // wTotalLength!!!!
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0xA0, // bmAttributes
    0x40, // bMaxPower
    // interface descriptor
    0x09, // bLength
    0x04, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bBakeInterface
    0x02, // bNumEndpoints
    0x08, // bInterfaceClass-mass storage
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // bStringIndex
    // bulk-in endpoint descriptor
    0x07, // bLength
    0x05, // bDescriptorType
    0x81, // bEndpointAddress
    0x02, // bmAttributes-bulk
    0x20, 0x00, // wMaxPacketSize
    0x00, // bInterval
    // bulk-out endpoint descriptor
    0x07, // bLength
    0x05, // bDescriptorType
    0x01, // bEndpointAddress
    0x02, // bmAttributes-bulk
    0x20, 0x00, // wMaxPacketSize
    0x00, // bInterval
}

static uint8_t msc_buffer[32];
static uint8_t *pDescr; // USB配置标志

/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void usb_device_init(void)
{
    IE_USB = 0;                                                                // 关闭USB中断
	USB_CTRL = 0x00;                                                           // 先设定USB设备模式
    // EndPoint 0
    UEP0_DMA = Ep0Buffer;                                                      // 端点0数据传输地址
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 // OUT事务返回ACK，IN事务返回NAK
    // EndPoint 1
    UEP1_DMA = Ep1Buffer;                                                      // 端点1数据传输地址
    UEP4_1_MOD = UEP4_1_MOD & ~bUEP1_BUF_MOD | bUEP1_TX_EN | bUEP1_RX_EN;      // 端点1 64字节发送(IN)+64字节接收(OUT)缓冲区
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;                 // 端点1自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
    // Other Init
    USB_DEV_AD = 0x00;
    UDEV_CTRL = bUD_PD_DIS;                                                    // 禁止DP/DM下拉电阻
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                      // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN;                                                  // 允许USB端口
    USB_INT_FG = 0xFF;                                                         // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;                  // 开启总线挂起、传输、总线复位中断、USB传输完成中断
    // IE_USB = 1;                                                                // 开启USB中断
}
