#ifndef __KEY_H
#define __KEY_H
#include"sys.h"
#define KEY0 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)  //��ȡ����0
#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)  //��ȡ����1
#define KEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)  //��ȡ����2
#define KEY3 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) //��ȡ����3
#define KEY4 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) //��ȡ����3

#define KEY0_PRES 1  //KEY0����
#define KEY1_PRES 2  //KEY1����
#define KEY2_PRES 3  //KEY2����
#define KEY3_PRES 4  //KEY3����
#define KEY4_PRES 5  //���ݰ�������

void KEY_Init(void);
u8 KEY_Scan(u8);
#endif 
