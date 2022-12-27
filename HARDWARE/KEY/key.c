#include"key.h"
#include"sys.h"
#include"delay.h"
void KEY_Init(void) //IO初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //设置上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//按键处理函数
//返回按键值
//mode: 0不支持连续按 ; 1支持连续按
//0 ，没有任何按键按下 1 KEY0 按下 2 KEY1 按下 3 KEY2 按下 4 KEY3 按下 WK_UP
//注意此函数有响应优先级, KEY0 > KEY1 > KEY2 > KEY3

u8 KEY_Scan(u8 mode)
{
	static u8 key_up = 1;
	if (mode){key_up = 1;delay_ms(100);};
	if (key_up && (KEY0 == 1 || KEY1 == 1 || KEY2 == 1 || KEY3 == 1|| KEY4 == 1))
	{
		delay_ms(10);
		key_up = 0;
		if (KEY0 == 1)return KEY0_PRES;
		else if (KEY1 == 1)return KEY1_PRES;
		else if (KEY2 == 1)return KEY2_PRES;
		else if (KEY3 == 1)return KEY3_PRES;
		else if (KEY4 == 1)return KEY4_PRES;
	}
	else if (KEY0 == 0 && KEY1 == 0 && KEY2 == 0 && KEY3 == 0 &&KEY4 == 0)key_up = 1;
	return 0;
}
