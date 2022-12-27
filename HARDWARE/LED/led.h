#ifndef __LED_H
#define __LED_H
#include "sys.h"
//LED 端口定义
#define LED0 PCout(13)
#define BEEP PCout(14)
void LED_Init(void);// 初始化
#endif
