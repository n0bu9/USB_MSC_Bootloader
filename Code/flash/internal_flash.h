#ifndef __INTERNAL_FLASH_H__
#define __INTERNAL_FLASH_H__

#include "stdint.h"

#define IFLASH_UDISK_START_ADDR   0x5000               // 20K

/* Check of Flash Operation */
#define DEF_FLASH_OP_CHECK1     0xAA
#define DEF_FLASH_OP_CHECK2     0x55

uint8_t write_code_flash(uint16_t addr, uint8_t *buf, uint16_t len);
void read_code_flash(uint16_t addr, uint8_t *buf, uint16_t len);

#endif
