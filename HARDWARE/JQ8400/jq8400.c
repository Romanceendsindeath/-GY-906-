#include "jq8400.h"   
#include "usart.h" 
#include "delay.h"

//���ź���  0-9��Ӧ����  10��ʮ   11����   12����   13����   14����������  15�������쳣   16����
void playnum(int num)
{
	usart_send_byte(0xAA);
	usart_send_byte(0x07);
	usart_send_byte(0x02);
	usart_send_byte(0x00);
	usart_send_byte(0x01+num);
	usart_send_byte(0xB4+num);
	delay_ms(400);
}

void playmusic(double Value)
{
		int bai,shi,ge,xiao=0;
		if(Value<0)
		{
			Value=-Value;
			playnum(12);
		}
	//�������λ
		bai=(int)(Value/100);
		shi=(int)(Value/10)%10;
		ge=(int)(Value)%10;
		xiao=(Value-(int)Value+0.00001)*10;
	//���Ÿ���λ
		if(bai!=0)
		{
			playnum(bai);
			playnum(11);
		}
		if(bai!=0 && shi==0)
		{
			playnum(0);
		}
		if(shi!=0)
		{
			playnum(shi);
			playnum(10);
		}
		playnum(ge);
		playnum(13);
		playnum(xiao);
		playnum(16);
}

