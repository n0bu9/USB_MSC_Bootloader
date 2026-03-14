#ifndef __USB_MSC_H__
#define __USB_MSC_H__

#include "ch554.h"
#include "stdint.h"

void usbfs_device_init(void);
void usbfs_device_polling(void);
uint8_t usbfs_all_descriptors_reported(void);

#endif // __USB_MSC_H__
