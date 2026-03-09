#include "ch554.h"
#include "system_ch55x.h"
#include "gpio.h"

sbit LED = P3^2;  // 定义LED连接在P3.2

void main()
{
    // 系统时钟设置为 24MHz
    system_init();

    // 其他初始化代码...
    port3_config(1, 2);  // 例如：将P1.0配置为推挽输出

    while (1)
    {
        LED = ~LED;  // 切换LED状态
        delay_ms(500);  // 延时500ms
    }
}

