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
	0x00,0x00,0x00,0x07,0x1F,0x30,0x20,0x40,0x40,0x40,0x40,0x40,0x20,0x10,0x00,0x00,/*"��",0*/	

};//OLED��ʾ���϶ȵ�λ����ֵ��

void ReadTemp(u8 *buf_w);  //��ȡ�¶Ⱥ���
void Keyaction(int key);   //������������  ������ְ�������
void LED_Temp();					//�����¶ȱ�����LED����˸
void OLED_ChoseShow(int modex);
void setsw();


u8 buf_w1[6];        //�¶Ȼ�����
double History[4]={0};   //������ʷ�¶�  ֻ��������Ĵ�
float value=0;//�¶�ֵ
float HTemp= 37.5;//�����¶�

int sign1 = 0;//���±�־λ 0δ���� 1�ѽ���
int sign2 = 0;//����־λ 0δ���� 1�ѽ���
int sign3 = 0;//������־λ 0δ���� 1�ѽ���
int sign4 = 0;//��ʷ�¶�   0δ���� 1�ѽ���
int sign5 = 0;//0Ϊʶ�������¶� 1Ϊʶ�������¶�
int main(void)
{		

	int key=0;      //�ж��ĸ�������������
	delay_init();	  //��ʱ������ʼ��	  
	OLED_Init()	;   //OLED��ʼ��
	OLED_Clear() ;  //����
	KEY_Init();     //������ʼ��
	LED_Init();     //LED��ʼ��
	uart_init(9600);
  MLX90614_IO_Init();  //������³�ʼ��
  First();             //��ʾ��ʼ����  
	setsw();//��������
	while(1)
	{	

		if(sign1==0&&sign2==0&&sign3==0&&sign4==0)value=0;   //���û�н����κ�һ��ģʽ�����������¶�����
		if(sign2==1)   //�������¶�ģʽ
		{
			ReadTemp(buf_w1);//������ȡ10���¶�ȡƽ��ֵ
			if(value<HTemp)OLED_ShowString(0,6,"          ",16);//�����趨�¶Ⱥ�ȡ�����¶ȹ��ߣ�������ʾ
			else	Fire();//��ʾ�¶ȹ��ߣ�
		}
		if(sign1==1)   //������ģʽ
		{
			if(value<HTemp)OLED_ShowString(0,6,"          ",16);  //�����趨�¶Ⱥ�ȡ�����¶ȹ��ߣ�������ʾ
			else	Fire();//��ʾ�¶ȹ��ߣ�
		}
		if(sign3==0)   //Ϊ�˷����޸��¶� �����뱨���¶��޸�ʱ ��������ģʽ û�н��뱨���¶��޸�ʱ ����������ģʽ
			key=KEY_Scan(0);//����ɨ�裬��ȡ��ǰʲô��������
		else key=KEY_Scan(1);  
		
		if(key!=0)   //key������0ʱ ������������
		{
			Keyaction(key); //���밴����������
		}	
		if(value!=0)LED_Temp();   //�����ȡ���¶� ����LED�ͷ��������жϲ���  ����0�൱��û�н����κ�ģʽ ��LED�ر�
		else 
		{
				LED0=0;   //�رշ�������LED
				BEEP=1;
		}
	}
}

//������������  ���õ����
void setsw()
{
	usart_send_byte(0xAA);
	usart_send_byte(0x13);
	usart_send_byte(0x01);
	usart_send_byte(0x1E);
	usart_send_byte(0xDC);
}

//�����¶Ⱥ���
void Keep_History(double Keep)   
{
		History[3]=History[2];// x->1 2 3 4   ==>  x 1 2 3   
		History[2]=History[1];
		History[1]=History[0];
		History[0]=Keep;
}

void LED_Temp()//LED��ʾ���� ������
{
		if(value>HTemp)  //������������¶� LED��˸ ����������
			{
				LED0=!LED0;
				BEEP=0;
				delay_ms(100);
			}else          //����رշ����� ������ָʾ�ƣ�LED��
			{
				LED0=1;
				BEEP=1;			
			}
}
//������еļ�ͷ 
void OLED_ClearChose()
{
	OLED_ShowString(80,0,"  ",16);
	OLED_ShowString(80,2,"  ",16);
	OLED_ShowString(80,4,"  ",16);
	OLED_ShowString(80,6,"  ",16);
}

//��ʾѡ���ͷ  
void OLED_ChoseShow(int modex)
{
	OLED_ClearChose();
	OLED_ShowString(80,2*modex,"<=",16);
}
//��ʾ�¶�  ��double�͵�value��Ϊ���� ��OLED��ʾ   xΪ��ʾ�ڵڼ��� һ������
void Temp(double Value,int x)
{
		uint8_t i,j;
		u8 buf_wx[6];
		int wend_h,wend_l;
		wend_h = Value;									//ȡ����������
		wend_l = (Value - wend_h)*10;   //ȡ��С������
		buf_wx[0]=wend_h/100+48;
		if(buf_wx[0]=='0')buf_wx[0]=' ';
		if(Value<0)
		{
			buf_wx[0]='-';
			Value=-Value;
			wend_h = Value;									//ȡ���������� С��0��ȡ����ֵ
			wend_l = (Value - wend_h)*10;   //ȡ��С������
		}
	  buf_wx[1]=wend_h/10+48;
		if((buf_wx[1]=='0' && buf_wx[0]=='0')||(buf_wx[1]=='0' && buf_wx[0]==' ')||(buf_wx[1]=='0' && buf_wx[0]=='-'))buf_wx[1]=' ';//�ж���λ��ʱʮλ�Ƿ�Ϊ0��Ϊ0��ȥ
		buf_wx[2]=wend_h%10+48;
		buf_wx[3]='.';
		buf_wx[4]=wend_l+48;
		buf_wx[5]='\0';		
		for(i = 6;i < 7;i++)//��ʾ�¶�
		{
			j = i - 6;
			OLED_ShowCHinese16x16(i*14,x*2,j,TempCompany);			//��ʾ���϶ȵ�λ
		}
		OLED_ShowString(40,x*2,(uint8_t *)buf_wx,16);					//��ʾ�¶�
}

//����ģʽ
void Mode1()
{
	sign1=1;  //��־λ ��1
	OLED_Clear();//�����Ļ�����ж���
	display();  //��ʾ���¶ȡ�
	OLED_Mode1();//��ʾ ���봥���������¡�
	People(sign5);
}
//�����¶�ģʽ
void Mode3()
{
	sign3=1;//��־λ
	OLED_Clear();
	display();
	Temp(HTemp,0);
}
//�鿴��ʷ�¶�ģʽ
void Mode4()
{
	sign4=1;
	OLED_Clear();
	His();
	OLED_Float(0,60,History[0],1);
	OLED_Float(2,60,History[1],1);
	OLED_Float(4,60,History[2],1);
	OLED_Float(6,60,History[3],1);//��ʾ��һ���ĸ���ʷ�¶�
}

void OLED_Action(int key)//ѡ��ģʽ����
{
	static int sign=0; //0,���� 1.��� 2.�趨�����¶� 3.��ʷ�¶�
	if(key==1)//ȫ����λ �������ʼ����
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
	if(key==2&&sign1==0&&sign2==0&&sign3==0&&sign4==0)//�����ƶ���ͷ
	{
		 sign++;//����
	}
	if(key==3&&sign1==0&&sign2==0&&sign3==0&&sign4==0)
	{
		 sign--;//����
	}
	if(key==4)//ȷ�ϼ�
	{
		 switch(sign)//�ж���ǰ��ͷָ���ĸ�ģʽ
		 {
			 case 0:if(sign1==0)playnum(18);delay_ms(600);Mode1();break;//0����ģʽ1
			 case 1:if(sign2==0)playnum(17);delay_ms(600);OLED_Clear();display();sign2=1;People(sign5);OLED_Mode2();break;//��ʾ�¶� ��������
			 case 2:OLED_ShowString(80,4,"  ",16);Mode3();break;//�����ʾ�������� ����ģʽ��
			 case 3:Mode4();break;
		 }
	}
	if((key==2&&sign1==1)||(key==2&&sign2==1)||(key==3&&sign1==1)||(key==3&&sign2==1))//�л��������� ��������λ
	{
			sign5=!sign5;
			People(sign5);
	}
	if(key==2&&sign3==1) //�����¶ȸı䣨֧�������ı��¶ȣ�  ÿ���޸�0.1�� д0.099����Ϊ���ݶ�ȡʱ��һ�����������Ƕ�35����35����������35.00000000000000000001
	{
		HTemp+=0.099;
		Temp(HTemp,0);
	}
	if(key==3&&sign3==1)
	{
		HTemp-=0.099;
		Temp(HTemp,0);
	}
	if(key==5&&sign1==1)//������ȴ�1s��ʼ����
	{
		Reading();
		delay_ms(1000);
		ReadTemp(buf_w1);
		Finish();
	}
	////////////////
	if(sign<0)  //����ģʽ�л�   ����������ģʽ
	{
		sign=3;
	}
	if(sign>3)
	{
		sign=0;
	}
	/////////////////
	if(sign1==0&&sign2==0&&sign3==0&&sign4==0)//˵������ѡ��״̬
	{
		switch(sign)
		{
			case 0:OLED_ChoseShow(0);break;//�����ƶ���ͷ
			case 1:OLED_ChoseShow(1);break;
			case 2:OLED_ChoseShow(2);break;
			case 3:OLED_ChoseShow(3);break;
		}
	}

}
void Keyaction(int key)//��������
{
	switch(key)
	{ 
		case 1:OLED_Action(1);break;  //���ҵİ���
		case 2:OLED_Action(2);break;	
		case 3:OLED_Action(3);break;
		case 4:OLED_Action(4);break;	//����İ���
		case 5:OLED_Action(5);break;  //���°���
	}
}
void ReadTemp(u8 *buf_w)//��ȡ�¶Ⱥ���
{
		uint8_t i,j,k;
		int wendu_h,wendu_l;
	  double num=0;
		for(k=0;k<100;k++)//������ȡ100��ȡƽ��ֵ
			num+=MLX90614_read_value();
		value = num/100.0;
		if(sign5==1)value+=3.5;
		if(sign1==1)Keep_History(value);//ֻ��¼����ģʽ�µ���ʷ�¶�
		wendu_h = value;
		wendu_l = (value - wendu_h)*10;
		buf_w[0]=wendu_h/100+48;
		if(buf_w[0]=='0')buf_w[0]=' ';
		if(value<0)
		{
			buf_w[0]='-';
			value=-value;
			wendu_h = value;									//ȡ���������� С��0��ȡ����ֵ
			wendu_l = (value - wendu_h)*10;   //ȡ��С������
		}
	  buf_w[1]=wendu_h/10+48;
		if((buf_w[1]=='0' && buf_w[0]=='0')||(buf_w[1]=='0' && buf_w[0]==' ')||(buf_w[1]=='0' && buf_w[0]=='-'))buf_w[1]=' ';//�ж���λ��ʱʮλ�Ƿ�Ϊ0��Ϊ0��ȥ
		buf_w[2]=wendu_h%10+48;
		buf_w[3]='.';
		buf_w[4]=wendu_l+48;
		buf_w[5]='\0';		
		for(i = 6;i < 7;i++)//��ʾ�¶ȵ�λ
		{
			j = i - 6;
			OLED_ShowCHinese16x16(i*14,0,j,TempCompany);			
		}
		OLED_ShowString(40,0,(uint8_t *)buf_w,16);//����OLED����ʾʵ�ʲ������¶�
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

