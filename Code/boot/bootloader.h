#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include "main.h"

typedef enum {
    BOOT_INIT = 0,
    BOOT_WAIT_USB,
    BOOT_IDLE,
    BOOT_RX_FILE,
    BOOT_PROGRAM,
    BOOT_VERIFY,
    BOOT_DONE,
    BOOT_ERROR
} boot_state_t;

void bootloader_main_task(void);

#endif  // __BOOTLOADER_H__
