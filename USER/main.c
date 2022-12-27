#include "stdio.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "mlx90614.h" 
#include "oled.h"
#include "key.h"
#include "led.h"
#include "usart.h"
#include "jq8400.h"

unsigned char TempCompany[][16]=
{
	0x06,0x09,0x09,0xE6,0xF8,0x0C,0x04,0x02,0x02,0x02,0x02,0x02,0x04,0x1E,0x00,0x00,
	0x00,0x00,0x00,0x07,0x1F,0x30,0x20,0x40,0x40,0x40,0x40,0x40,0x20,0x10,0x00,0x00,/*"℃",0*/	

};//OLED显示摄氏度单位的真值表

void ReadTemp(u8 *buf_w);  //读取温度函数
void Keyaction(int key);   //按键动作函数  处理各种按键操作
void LED_Temp();					//控制温度报警和LED灯闪烁
void OLED_ChoseShow(int modex);
void setsw();


u8 buf_w1[6];        //温度缓存区
double History[4]={0};   //保存历史温度  只保留最近四次
float value=0;//温度值
float HTemp= 37.5;//报警温度

int sign1 = 0;//测温标志位 0未进入 1已进入
int sign2 = 0;//检测标志位 0未进入 1已进入
int sign3 = 0;//报警标志位 0未进入 1已进入
int sign4 = 0;//历史温度   0未进入 1已进入
int sign5 = 0;//0为识别物体温度 1为识别人物温度
int main(void)
{		

	int key=0;      //判定哪个按键发生动作
	delay_init();	  //延时函数初始化	  
	OLED_Init()	;   //OLED初始化
	OLED_Clear() ;  //清屏
	KEY_Init();     //按键初始化
	LED_Init();     //LED初始化
	uart_init(9600);
  MLX90614_IO_Init();  //红外测温初始化
  First();             //显示初始界面  
	setsw();//设置音量
	while(1)
	{	

		if(sign1==0&&sign2==0&&sign3==0&&sign4==0)value=0;   //如果没有进入任何一个模式就清除保存的温度数据
		if(sign2==1)   //进入检测温度模式
		{
			ReadTemp(buf_w1);//连续读取10次温度取平均值
			if(value<HTemp)OLED_ShowString(0,6,"          ",16);//低于设定温度后取消“温度过高！”的显示
			else	Fire();//显示温度过高！
		}
		if(sign1==1)   //进入检测模式
		{
			if(value<HTemp)OLED_ShowString(0,6,"          ",16);  //低于设定温度后取消“温度过高！”的显示
			else	Fire();//显示温度过高！
		}
		if(sign3==0)   //为了方便修改温度 当进入报警温度修改时 采用连按模式 没有进入报警温度修改时 不采用连按模式
			key=KEY_Scan(0);//按键扫描，获取当前什么按键按下
		else key=KEY_Scan(1);  
		
		if(key!=0)   //key不等于0时 按键发生动作
		{
			Keyaction(key); //进入按键动作函数
		}	
		if(value!=0)LED_Temp();   //如果读取到温度 进行LED和蜂鸣器的判断操作  等于0相当于没有进入任何模式 将LED关闭
		else 
		{
				LED0=0;   //关闭蜂鸣器和LED
				BEEP=1;
		}
	}
}

//设置音量函数  设置到最大
void setsw()
{
	usart_send_byte(0xAA);
	usart_send_byte(0x13);
	usart_send_byte(0x01);
	usart_send_byte(0x1E);
	usart_send_byte(0xDC);
}

//保存温度函数
void Keep_History(double Keep)   
{
		History[3]=History[2];// x->1 2 3 4   ==>  x 1 2 3   
		History[2]=History[1];
		History[1]=History[0];
		History[0]=Keep;
}

void LED_Temp()//LED显示部分 蜂鸣器
{
		if(value>HTemp)  //如果超出报警温度 LED闪烁 蜂鸣器发声
			{
				LED0=!LED0;
				BEEP=0;
				delay_ms(100);
			}else          //否则关闭蜂鸣器 并常亮指示灯（LED）
			{
				LED0=1;
				BEEP=1;			
			}
}
//清除各行的箭头 
void OLED_ClearChose()
{
	OLED_ShowString(80,0,"  ",16);
	OLED_ShowString(80,2,"  ",16);
	OLED_ShowString(80,4,"  ",16);
	OLED_ShowString(80,6,"  ",16);
}

//显示选择箭头  
void OLED_ChoseShow(int modex)
{
	OLED_ClearChose();
	OLED_ShowString(80,2*modex,"<=",16);
}
//显示温度  将double型的value变为数组 给OLED显示   x为显示在第几行 一共四行
void Temp(double Value,int x)
{
		uint8_t i,j;
		u8 buf_wx[6];
		int wend_h,wend_l;
		wend_h = Value;									//取出整数部分
		wend_l = (Value - wend_h)*10;   //取出小数部分
		buf_wx[0]=wend_h/100+48;
		if(buf_wx[0]=='0')buf_wx[0]=' ';
		if(Value<0)
		{
			buf_wx[0]='-';
			Value=-Value;
			wend_h = Value;									//取出整数部分 小于0是取反后赋值
			wend_l = (Value - wend_h)*10;   //取出小数部分
		}
	  buf_wx[1]=wend_h/10+48;
		if((buf_wx[1]=='0' && buf_wx[0]=='0')||(buf_wx[1]=='0' && buf_wx[0]==' ')||(buf_wx[1]=='0' && buf_wx[0]=='-'))buf_wx[1]=' ';//判定两位数时十位是否为0，为0消去
		buf_wx[2]=wend_h%10+48;
		buf_wx[3]='.';
		buf_wx[4]=wend_l+48;
		buf_wx[5]='\0';		
		for(i = 6;i < 7;i++)//显示温度
		{
			j = i - 6;
			OLED_ShowCHinese16x16(i*14,x*2,j,TempCompany);			//显示摄氏度单位
		}
		OLED_ShowString(40,x*2,(uint8_t *)buf_wx,16);					//显示温度
}

//测温模式
void Mode1()
{
	sign1=1;  //标志位 置1
	OLED_Clear();//清除屏幕上所有东西
	display();  //显示“温度”
	OLED_Mode1();//显示 “请触摸按键测温”
	People(sign5);
}
//设置温度模式
void Mode3()
{
	sign3=1;//标志位
	OLED_Clear();
	display();
	Temp(HTemp,0);
}
//查看历史温度模式
void Mode4()
{
	sign4=1;
	OLED_Clear();
	His();
	OLED_Float(0,60,History[0],1);
	OLED_Float(2,60,History[1],1);
	OLED_Float(4,60,History[2],1);
	OLED_Float(6,60,History[3],1);//显示第一到四个历史温度
}

void OLED_Action(int key)//选择模式设置
{
	static int sign=0; //0,测温 1.检测 2.设定报警温度 3.历史温度
	if(key==1)//全部复位 并进入初始界面
	{		
		sign1=0;
		sign2=0;
		sign3=0;
		sign4=0;
		sign=0;
		LED0=0;
		BEEP=1;
		OLED_Clear();
		First();
		OLED_ClearChose();
		OLED_ChoseShow(0);
	}
	if(key==2&&sign1==0&&sign2==0&&sign3==0&&sign4==0)//上下移动箭头
	{
		 sign++;//下移
	}
	if(key==3&&sign1==0&&sign2==0&&sign3==0&&sign4==0)
	{
		 sign--;//上移
	}
	if(key==4)//确认键
	{
		 switch(sign)//判定当前箭头指向哪个模式
		 {
			 case 0:if(sign1==0)playnum(18);delay_ms(600);Mode1();break;//0进入模式1
			 case 1:if(sign2==0)playnum(17);delay_ms(600);OLED_Clear();display();sign2=1;People(sign5);OLED_Mode2();break;//显示温度 测量对象
			 case 2:OLED_ShowString(80,4,"  ",16);Mode3();break;//清除显示测量对象 进入模式三
			 case 3:Mode4();break;
		 }
	}
	if((key==2&&sign1==1)||(key==2&&sign2==1)||(key==3&&sign1==1)||(key==3&&sign2==1))//切换测量对象 二，三键位
	{
			sign5=!sign5;
			People(sign5);
	}
	if(key==2&&sign3==1) //报警温度改变（支持连按改变温度）  每次修改0.1度 写0.099是因为数据读取时有一定的误差，并不是读35就是35，而可能是35.00000000000000000001
	{
		HTemp+=0.099;
		Temp(HTemp,0);
	}
	if(key==3&&sign3==1)
	{
		HTemp-=0.099;
		Temp(HTemp,0);
	}
	if(key==5&&sign1==1)//触摸后等待1s开始测温
	{
		Reading();
		delay_ms(1000);
		ReadTemp(buf_w1);
		Finish();
	}
	////////////////
	if(sign<0)  //三种模式切换   不超过三种模式
	{
		sign=3;
	}
	if(sign>3)
	{
		sign=0;
	}
	/////////////////
	if(sign1==0&&sign2==0&&sign3==0&&sign4==0)//说明还在选择状态
	{
		switch(sign)
		{
			case 0:OLED_ChoseShow(0);break;//上下移动箭头
			case 1:OLED_ChoseShow(1);break;
			case 2:OLED_ChoseShow(2);break;
			case 3:OLED_ChoseShow(3);break;
		}
	}

}
void Keyaction(int key)//按键动作
{
	switch(key)
	{ 
		case 1:OLED_Action(1);break;  //最右的按键
		case 2:OLED_Action(2);break;	
		case 3:OLED_Action(3);break;
		case 4:OLED_Action(4);break;	//最左的按键
		case 5:OLED_Action(5);break;  //测温按键
	}
}
void ReadTemp(u8 *buf_w)//读取温度函数
{
		uint8_t i,j,k;
		int wendu_h,wendu_l;
	  double num=0;
		for(k=0;k<100;k++)//连续读取100次取平均值
			num+=MLX90614_read_value();
		value = num/100.0;
		if(sign5==1)value+=3.5;
		if(sign1==1)Keep_History(value);//只记录测温模式下的历史温度
		wendu_h = value;
		wendu_l = (value - wendu_h)*10;
		buf_w[0]=wendu_h/100+48;
		if(buf_w[0]=='0')buf_w[0]=' ';
		if(value<0)
		{
			buf_w[0]='-';
			value=-value;
			wendu_h = value;									//取出整数部分 小于0是取反后赋值
			wendu_l = (value - wendu_h)*10;   //取出小数部分
		}
	  buf_w[1]=wendu_h/10+48;
		if((buf_w[1]=='0' && buf_w[0]=='0')||(buf_w[1]=='0' && buf_w[0]==' ')||(buf_w[1]=='0' && buf_w[0]=='-'))buf_w[1]=' ';//判定两位数时十位是否为0，为0消去
		buf_w[2]=wendu_h%10+48;
		buf_w[3]='.';
		buf_w[4]=wendu_l+48;
		buf_w[5]='\0';		
		for(i = 6;i < 7;i++)//显示温度单位
		{
			j = i - 6;
			OLED_ShowCHinese16x16(i*14,0,j,TempCompany);			
		}
		OLED_ShowString(40,0,(uint8_t *)buf_w,16);//在在OLED上显示实际测量的温度
		if(sign1==1)
		{
			playmusic(value);
		}
		if(sign1==1 && sign5==1)
		{
			if(value<HTemp)
				playnum(14);
			else
				playnum(15);
		}
}

