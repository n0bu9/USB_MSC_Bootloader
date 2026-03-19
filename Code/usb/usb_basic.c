#include "usb_basic.h"
#include "usb_scsi.h"
#include "string.h"

#define THIS_ENDP0_SIZE		32UL
#define THIS_ENDP2_SIZE		32UL
#define THIS_ENDP2_BUF_SIZE 64UL
#define UsbSetupBuf     ((PUSB_SETUP_REQ)ep0_buffer)

#define USB_DESC_REPORTED_DEVICE    0x01
#define USB_DESC_REPORTED_CONFIG    0x02

static uint8_t xdata ep0_buffer[THIS_ENDP0_SIZE+2] _at_ 0x0000;    //端点0 OUT&IN缓冲区，必须是偶地址
static uint8_t xdata ep2_buffer[THIS_ENDP2_SIZE+2] _at_ THIS_ENDP0_SIZE+2;  //端点1 OUT&IN缓冲区，必须是偶地址

static uint8_t xdata ep2_in_buffer[THIS_ENDP2_BUF_SIZE] _at_ ( THIS_ENDP0_SIZE + 2 + THIS_ENDP2_BUF_SIZE );
#define ep2_out_buffer    ( ep2_buffer )

uint8_t device_descriptor[] = {
    0x12,  // bLength
    0x01,  // bDescriptorType
    0x10, 0x01,  // usb1.1
    0x00, // bDeviceClass-mass storage
    0x00, // bDeviceSubClass
    0x00, // bDeviceProtocol
    THIS_ENDP0_SIZE, // bMaxPacketSize0
    0x1A, 0x86, // idVendor
    0xFE, 0x10, // idProduct
    0x01, 0x00, // bcdDevice
    0x00, // iManufacturer
    0x00, // iProduct
    0x00, // iSerialNumber
    0x01  // bNumConfigurations
};

uint8_t configuration_descriptor[] = {
    // configuration descriptor
    0x09, // bLength
    0x02, // bDescriptorType
    0x20, 0x00, // wTotalLength
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0xA0, // bmAttributes
    0x32, // bMaxPower
    // interface descriptor
    0x09, // bLength
    0x04, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bBakeInterface
    0x02, // bNumEndpoints
    0x08, // bInterfaceClass-mass storage
    0x06, // bInterfaceSubClass
    0x50, // bInterfaceProtocol
    0x00, // bStringIndex
    // bulk-in endpoint descriptor
    0x07, // bLength
    0x05, // bDescriptorType
    0x82, // bEndpointAddress
    0x02, // bmAttributes-bulk
    0x20, 0x00, // wMaxPacketSize
    0x00, // bInterval
    // bulk-out endpoint descriptor
    0x07, // bLength
    0x05, // bDescriptorType
    0x02, // bEndpointAddress
    0x02, // bmAttributes-bulk
    0x20, 0x00, // wMaxPacketSize
    0x00, // bInterval
};

static uint8_t usb_inbuffer[32];
static uint8_t usb_outbuffer[32];
static uint8_t *pDescr; // USB配置标志
static uint8_t usb_descriptor_report_mask = 0;
static uint8_t usb_descriptor_pending_mask = 0;
// static uint8_t SetupReq = 0,SetupLen = 0,Ready = 0,Count = 0,UsbConfig = 0;

/*******************************************************************************
* Function Name  : usbfs_device_init()
* Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void usbfs_device_init(void)
{
    IE_USB = 0;                                                                // 关闭USB中断
	USB_CTRL = 0x00;                                                           // 先设定USB设备模式
    // EndPoint 0
    UEP0_DMA = ep0_buffer;                                                     // 端点0数据传输地址
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 // OUT事务返回ACK，IN事务返回NAK
    // EndPoint 2
    UEP2_DMA = ep2_buffer;                                                     // 端点2数据传输地址
    UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN | bUEP2_RX_EN;      // 端点2 32字节发送(IN)+32字节接收(OUT)缓冲区
    UEP2_CTRL = UEP_T_RES_NAK | UEP_R_RES_ACK;                                 // 端点2双向同时启用时需手动翻转toggle
    // Other Init
    USB_DEV_AD = 0x00;
    UDEV_CTRL = bUD_PD_DIS;                                                    // 禁止DP/DM下拉电阻
    USB_CTRL = bUC_DEV_PU_EN | bUC_DMA_EN | bUC_INT_BUSY;                      // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN;                                                  // 允许USB端口
    USB_INT_FG = 0xFF;                                                         // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;                  // 开启总线挂起、传输、总线复位中断、USB传输完成中断
    usb_descriptor_report_mask = 0;
    usb_descriptor_pending_mask = 0;
    UDISK_Init();
    IE_USB = 1;                                                                // 开启USB中断
}

uint8_t usbfs_all_descriptors_reported(void)
{
    return (usb_descriptor_report_mask == (USB_DESC_REPORTED_DEVICE | USB_DESC_REPORTED_CONFIG)) ? 1 : 0;

}
void SetEPTxStatus(uint8_t bEpnum, uint8_t wState)
{
    switch (bEpnum)
    {
    case EP_NUM_0:
        _SetEPTxStatus(UEP0_CTRL, wState);
        break;
    case EP_NUM_1:
        _SetEPTxStatus(UEP1_CTRL, wState);
        break;
    case EP_NUM_2:
        _SetEPTxStatus(UEP2_CTRL, wState);
        break;
    case EP_NUM_3:
        _SetEPTxStatus(UEP3_CTRL, wState);
        break;
    default:
        break;
    }
}

void SetEPRxStatus(uint8_t bEpnum, uint8_t wState)
{
    switch (bEpnum)
    {
    case EP_NUM_0:
        _SetEPRxStatus(UEP0_CTRL, wState);
        break;
    case EP_NUM_1:
        _SetEPRxStatus(UEP1_CTRL, wState);
        break;
    case EP_NUM_2:
        _SetEPRxStatus(UEP2_CTRL, wState);
        break;
    case EP_NUM_3:
        _SetEPRxStatus(UEP3_CTRL, wState);
        break;
    default:
        break;
    }
}

/*******************************************************************************
 * @fn                USB_SIL_Write
 *
 * @brief           Write a buffer of data to a selected endpoint.
 *
 * @param          bEpAddr: The address of the non control endpoint.
 *                  pBufferPointer: The pointer to the buffer of data to be written
 *                     to the endpoint.
 *                  wBufferSize: Number of data to be written (in bytes).
 *
 * @return         Status.
 */
#define ep2_buffer ep2_in_buffer
void USB_SIL_Write(uint8_t bEpnum, uint8_t* pBufferPointer, uint32_t wBufferSize)
{
    switch (bEpnum)
    {
    case EP_NUM_0:
        memcpy(ep0_buffer, pBufferPointer, wBufferSize);                                // 加载上传数据
        UEP0_T_LEN = wBufferSize;                                                       // 上传数据长度
        break;
    case EP_NUM_2:
        memcpy(ep2_buffer, pBufferPointer, wBufferSize);                                // 加载上传数据
        UEP2_T_LEN = wBufferSize;                                                       // 上传数据长度
        break;
    default:
        return;
    }
    SetEPTxStatus(bEpnum, EP_TX_ACK);
}
#undef ep2_buffer

void usbfs_device_interrupt(void) interrupt INT_NO_USB using 1
{
    uint8_t len, desc_len, request_type, request_recip;
    static uint8_t SetupReqCode, SetupLen, UsbConfig;
    static uint8_t *pDescr;
    if(UIF_TRANSFER)                                                // USB传输完成
    {
        if (U_IS_NAK){}                                             // 本例子可以不必处理NAK
        else
        {
            switch ( USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) )// 分析操作令牌和端点号
            {
            case UIS_TOKEN_OUT | 2:                                   // endpoint 2# 批量端点下传
                if ( U_TOG_OK ) {
                    UEP2_CTRL ^= bUEP_R_TOG;
                    len = USB_RX_LEN;
                    UDISK_Out_EP_Deal(ep2_out_buffer, len);
                    SetEPRxStatus(EP_NUM_2, EP_RX_VALID);
                    break;
                }
                break;
            case UIS_TOKEN_IN | 2:                                    // endpoint 2# 批量端点上传
                UEP2_CTRL ^= bUEP_T_TOG;
                SetEPTxStatus(EP_NUM_2, EP_TX_NAK);
                UDISK_In_EP_Deal();
                break;
            case UIS_TOKEN_SETUP | 0:                                 // endpoint 0# SETUP
                len = USB_RX_LEN;
                if (len == sizeof(USB_SETUP_REQ))                 // SETUP包长度
                {
                    request_type = UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK;
                    request_recip = UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK;
                    if ( ( request_type == USB_REQ_TYP_CLASS ) && ( request_recip == USB_REQ_RECIP_INTERF ) && ( UsbSetupBuf->wIndexL == 0x00 ) )
                    {
                        SetupReqCode = UsbSetupBuf->bRequest;
                        SetupLen = UsbSetupBuf->wLengthL;
                        len = 0;
                        desc_len = 0;
                        usb_descriptor_pending_mask = 0;
                        switch( SetupReqCode )
                        {
                        case CMD_UDISK_GET_MAX_LUN:
                            if ( ( UsbSetupBuf->bRequestType & 0x80 ) && ( SetupLen == 1 ) )
                            {
                                ep0_buffer[ 0 ] = 0x00;
                                len = 1;
                            }
                            else
                            {
                                len = 0xFF;
                            }
                            break;
                        case CMD_UDISK_RESET:
                            if ( ( ( UsbSetupBuf->bRequestType & 0x80 ) == 0x00 ) && ( SetupLen == 0 ) )
                            {
                                UDISK_Reset_Bulk_State( );
                                SetEPTxStatus( EP_NUM_2, EP_TX_NAK );
                                SetEPRxStatus( EP_NUM_2, EP_RX_VALID );
                                len = 0;
                            }
                            else
                            {
                                len = 0xFF;
                            }
                            break;
                        default:
                            len = 0xFF;
                            break;
                        }
                    }
                    else if ( request_type != USB_REQ_TYP_STANDARD )
                    {
                        len = 0xFF;                                   // 操作失败
                    }
                    else                                              // 标准请求
                    {
                        SetupReqCode = UsbSetupBuf->bRequest;
                        SetupLen = UsbSetupBuf->wLengthL;
                        len = 0;
                        desc_len = 0;
                        usb_descriptor_pending_mask = 0;
                        switch( SetupReqCode )                        // 请求码
                        {
                        case USB_GET_DESCRIPTOR:
                            switch( UsbSetupBuf->wValueH )
                            {
                            case 1:                                   // 设备描述符
                                pDescr = device_descriptor;
                                desc_len = sizeof(device_descriptor);
                                len = desc_len;
                                if ( SetupLen >= desc_len )
                                {
                                    usb_descriptor_pending_mask = USB_DESC_REPORTED_DEVICE;
                                }
                                break;
                            case 2:                                   // 配置描述符
                                pDescr = configuration_descriptor;
                                desc_len = sizeof(configuration_descriptor);
                                len = desc_len;
                                if ( SetupLen >= desc_len )
                                {
                                    usb_descriptor_pending_mask = USB_DESC_REPORTED_CONFIG;
                                }
                                break;
                            default:
                                len = 0xFF;                           // 不支持的描述符类型
                                break;
                            }
                            if ( SetupLen > len )
                            {
                                SetupLen = len;                       // 限制总长度
                            }
                            len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;// 本次传输长度
                            memcpy(ep0_buffer, pDescr, len);         // 加载上传数据
                            SetupLen -= len;
                            pDescr += len;
                            break;
                        case USB_SET_ADDRESS:
                            SetupLen = UsbSetupBuf->wValueL;          // 暂存USB设备地址
                            break;
                        case USB_GET_CONFIGURATION:
                            ep0_buffer[0] = UsbConfig;
                            if ( SetupLen >= 1 )
                            {
                                len = 1;
                            }
                            break;
                        case USB_SET_CONFIGURATION:
                            UsbConfig = UsbSetupBuf->wValueL;
                            break;
                        default:
                            len = 0xFF;                               // 操作失败
                            break;
                        }
                    }
                }
                else
                {
                    len = 0xFF;                                       // SETUP包长度错误
                }
                if ( len == 0xFF )                                    // 操作失败
                {
                    SetupReqCode = 0xFF;
                    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;// STALL
                }
                else if ( len <= THIS_ENDP0_SIZE )                    // 上传数据或者状态阶段返回0长度包
                {
                    UEP0_T_LEN = len;
                    UEP0_CTRL  = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// 默认数据包是DATA1
                }
                else                                                  // 下传数据或其它
                {
                    UEP0_T_LEN = 0;                                   // 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// 默认数据包是DATA1
                }
                break;
            case UIS_TOKEN_IN | 0:                                    // endpoint 0# IN
                switch( SetupReqCode )
                {
                case USB_GET_DESCRIPTOR:
                    len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;// 本次传输长度
                    memcpy(ep0_buffer, pDescr, len);                 // 加载上传数据
                    SetupLen -= len;
                    pDescr += len;
                    UEP0_T_LEN = len;
                    UEP0_CTRL ^= bUEP_T_TOG;                          // 翻转
                    break;
                case USB_SET_ADDRESS:
                    USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;
                default:
                    UEP0_T_LEN = 0;                                   // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;
                }
                break;
            case UIS_TOKEN_OUT | 0:                                   // endpoint 0# OUT
                switch( SetupReqCode )
                {
                case USB_GET_DESCRIPTOR:
                    if ( usb_descriptor_pending_mask != 0 )
                    {
                        usb_descriptor_report_mask |= usb_descriptor_pending_mask;
                        usb_descriptor_pending_mask = 0;
                    }
                    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;
                default:
                    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;        // 准备下一控制传输
                    break;
                }
                break;
            default:
                break;
            }
        }
        UIF_TRANSFER = 0;                                             // 清中断标志
    }
    else if ( UIF_BUS_RST )                                           // USB总线复位
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        // UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
        UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UsbConfig = 0;
        usb_descriptor_report_mask = 0;
        usb_descriptor_pending_mask = 0;
        UDISK_Init();
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;                                              // 清中断标志
    }
    else if ( UIF_SUSPEND )                                           // USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
    }
    else                                                              // 意外的中断,不可能发生的情况
    {
        USB_INT_FG = 0xFF;                                            // 清中断标志
    }
}
