#ifndef __USB_BASIC_H__
#define __USB_BASIC_H__

#include "ch554.h"
#include "stdint.h"

#define EP_NUM_0       0
#define EP_NUM_1       1
#define EP_NUM_2       2
#define EP_NUM_3       3
#define EP_TX_ACK      (UEP_T_RES_ACK)   /* Endpoint Tx ACK */
#define EP_TX_VALID    (UEP_T_RES_ACK)   /* Endpoint Tx data ready */
#define EP_TX_NAK      (UEP_T_RES_NAK)   /* EndPoint TX NAK */
#define EP_TX_STALL    (UEP_T_RES_STALL) /* EndPoint TX STALL */
#define EP_RX_ACK      (UEP_R_RES_ACK)   /* Endpoint Rx ACK */
#define EP_RX_VALID    (UEP_R_RES_ACK)   /* Endpoint Rx ready */
#define EP_RX_NAK      (UEP_R_RES_NAK)   /* EndPoint RX NAK */
#define EP_RX_STALL    (UEP_R_RES_STALL) /* EndPoint RX STALL */
/*******************************************************************************
* Macro Name     : SetEPTxStatus
* Description    : sets the status for tx transfer
* Input          : bEpNum: Endpoint Number.
*                  wState: new state
* Return         : None.
*******************************************************************************/
#define _SetEPTxStatus(reg, wState)  do{\
                                        (reg) = ((reg) & ~MASK_UEP_T_RES) | (wState);\
                                    } while(0)

/*******************************************************************************
* Macro Name     : SetEPRxStatus
* Description    : sets the status for rx transfer
* Input          : bEpNum: Endpoint Number.
*                  wState: new state.
* Return         : None.
*******************************************************************************/
#define _SetEPRxStatus(reg, wState)  do{\
                                        (reg) = ((reg) & ~MASK_UEP_R_RES) | (wState);\
                                    } while(0)
// #define USB_SIL_Write

void usbfs_device_init(void);
uint8_t usbfs_all_descriptors_reported(void);
void SetEPTxStatus(uint8_t bEpnum, uint8_t wState);
void SetEPRxStatus(uint8_t bEpnum, uint8_t wState);
void USB_SIL_Write(uint8_t bEpnum, uint8_t* pBufferPointer, uint32_t wBufferSize);

#endif // __USB_BASIC_H__
