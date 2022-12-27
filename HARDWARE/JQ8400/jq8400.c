#include "jq8400.h"   
#include "usart.h" 
#include "delay.h"

//播放函数  0-9对应数字  10：十   11：百   12：负   13：点   14：体温正常  15：体温异常   16：度
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
	//提出各个位
		bai=(int)(Value/100);
		shi=(int)(Value/10)%10;
		ge=(int)(Value)%10;
		xiao=(Value-(int)Value+0.00001)*10;
	//播放各个位
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

