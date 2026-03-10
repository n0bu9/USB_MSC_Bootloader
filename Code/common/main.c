#include "main.h"
#include "ch554.h"
#include "system_ch55x.h"
#include "bootloader.h"

void main()
{
    system_init();

    bootloader_main_task();
}
