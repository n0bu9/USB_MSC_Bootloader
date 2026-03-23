#include "internal_flash.h"

uint8_t flash_op_check_byte1 = 0x00;
uint8_t flash_op_check_byte2 = 0x00;

uint8_t flash_op_unlock(void)
{
    bit ea_state_save = 0;

    /* Check the Flash operation flags to prevent Flash misoperation. */
    if( ( flash_op_check_byte1 != DEF_FLASH_OP_CHECK1 ) ||
        ( flash_op_check_byte2 != DEF_FLASH_OP_CHECK2 ) ) return 0xff;  /* Flash Operation Flags Error */

    ea_state_save = EA;
    EA = 1;

    /* Enable Flash writing operations*/
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xaa;
    GLOBAL_CFG |= bCODE_WE;
    SAFE_MOD = 0x00;

    /* Restore all INTS */
    EA = ea_state_save;
    return 0x00;
}

void flash_op_lock(void)
{
    bit ea_state_save = 0;

    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_state_save = EA;
    EA = 0;

    /* Disable Flash writing operations */
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xaa;
    GLOBAL_CFG &= ~bCODE_WE;
    // SAFE_MOD = 0x00;

    /* Restore all INTS */
    EA = ea_state_save;
}

uint8_t write_code_flash(uint16_t addr, uint8_t *buf, uint16_t len)
{
    if (flash_op_unlock()) return 0xff;


    while (len--)
    {
        ROM_ADDR_L = (uint8_t)(addr & 0xff);
        ROM_ADDR_H = (uint8_t)(addr >> 8);
        ROM_DATA_L = *buf;
        ROM_DATA_H = *(buf + sizeof(uint8_t));
        buf = buf + sizeof(uint8_t)*2;
        addr = addr + 2;
        ROM_CTRL = 0x09;  // do write operation
        if (!(ROM_STATUS & 0x40))
        {
            flash_op_lock();
            return 0xff;  // check write operation if sunccess
        }
    }

    flash_op_lock();
    return 0x00;
}

void check_code_flash(uint16_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t *p;
    p = addr;
}
