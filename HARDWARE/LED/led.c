#include "led.h"
#include"key.h"
#include"sys.h"
#include"delay.h"
//��ʼ�� ����� ��ʹ���������ڵ�ʱ��
//LED BEEP ��ʼ��
void LED_Init(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //ʹ�� PB,PE �˿�ʱ��
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14; 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC, &GPIO_InitStructure);
GPIO_SetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14); //PC.13 �����

}
