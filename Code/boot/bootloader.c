#include "bootloader.h"
#include "system_ch55x.h"
#include "bsp_led.h"

typedef void(*pTaskFn)(void);
pTaskFn userTask;

void __bl_led_task(boot_state_t boot_state)
{
    switch (boot_state)
    {
    case BOOT_WAIT_USB:
        /* code */
        break;

    default:
        break;
    }
}

void __bl_states_task(boot_state_t *boot_state)
{
    switch (*boot_state)
    {
    case BOOT_INIT:
        Port_LED_Init();
        *boot_state = BOOT_WAIT_USB;
        break;

    case BOOT_WAIT_USB:
        LED_ON();
        delay_ms(500);
        LED_OFF();
        delay_ms(500);
        // 等待USB连接，连接成功后切换到BOOT_IDLE
        // if (usb_connected()) {
        //     *boot_state = BOOT_IDLE;
        // }
        break;

    case BOOT_IDLE:
        // 等待主机发送文件，收到文件后切换到BOOT_RX_FILE
        // if (file_received()) {
        //     *boot_state = BOOT_RX_FILE;
        // }
        break;

    case BOOT_RX_FILE:
        // 接收文件，接收完成后切换到BOOT_PROGRAM
        // if (file_received_complete()) {
        //     *boot_state = BOOT_PROGRAM;
        // }
        break;

    case BOOT_PROGRAM:
        // 编程Flash，编程完成后切换到BOOT_VERIFY
        // if (program_complete()) {
        //     *boot_state = BOOT_VERIFY;
        // }
        break;

    case BOOT_VERIFY:
        // 验证Flash，验证成功后切换到BOOT_DONE，失败则切换到BOOT_ERROR
        break;


    default:
        break;
    }
}

void bootloader_jump_to_app()
{
    // 定义函数指针，指向应用程序的入口地址
    userTask = (pTaskFn)APP_START_ADDR;
    // 关闭所有外设，复位系统状态
    // 例如：关闭USB、定时器等

    // 跳转到应用程序
    userTask();
}

void bootloader_main_task(void)
{
    boot_state_t boot_state = BOOT_INIT;

    while (1)
    {
        __bl_states_task(&boot_state);
    }
}
