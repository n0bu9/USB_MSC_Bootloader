#ifndef __USB_BASIC_H__
#define __USB_BASIC_H__

#include "ch554.h"
#include "stdint.h"

void usbfs_device_init(void);
uint8_t usbfs_all_descriptors_reported(void);

#endif // __USB_BASIC_H__
