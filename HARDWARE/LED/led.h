#ifndef __LED_H
#define __LED_H
#include "sys.h"
//LED �˿ڶ���
#define LED0 PCout(13)
#define BEEP PCout(14)
void LED_Init(void);// ��ʼ��
#endif