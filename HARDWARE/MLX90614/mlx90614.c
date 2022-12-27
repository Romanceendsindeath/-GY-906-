#include "mlx90614.h"

u8 value_H,value_L,PEC;


/***************�ܽŵĳ�ʼ��*************************
*MLX90614_SDA;PA2    ��©���8
*MLX90614_SCL;PB9   �������
***************************************************/
void MLX90614_IO_Init()
{
	//��GPIOBʱ��
	RCC->APB2ENR |= (0x1 << 3);
	RCC->APB2ENR |= (0x1 << 2);
	
	//��PB9����Ϊ�������
	GPIOB->CRH &= 0xffffff0f;
	GPIOB->CRH |= 0x00000030;	
	
	/*PA2 ��©���*/	
	GPIOA->CRL &= ~(0xf<<8);
	GPIOA->CRL |= 0x7<<8;
	
	MLX90614_SDA_out=1;
	MLX90614_SCL=1;
}


/*******����ʼλ*********/
void MLX90614_SDA_Start()
{	
	MLX90614_SDA_out = 1;
	Delay_us(1);
	MLX90614_SCL = 1;
	Delay_us(5);
	MLX90614_SDA_out = 0;
	Delay_us(10);
	MLX90614_SCL = 0;	
	Delay_us(4);
}

/*********������λ********/
void MLX90614_SDA_Stop()
{
	MLX90614_SCL = 0;
	Delay_us(5);
	MLX90614_SDA_out = 0;
	Delay_us(5);
	MLX90614_SCL = 1;
	Delay_us(10);
	MLX90614_SDA_out = 1;
	
}


/*����һ��λ*/
void MLX90614_SDA_send_bit(u8 bit_out)
{	
	if(bit_out == 1)
		MLX90614_SDA_out = 1;
	else
		MLX90614_SDA_out = 0;
	Delay_us(5);
	MLX90614_SCL = 1;
	Delay_us(10);
	MLX90614_SCL = 0;	
	Delay_us(4);
}



/***  �� �� ������һ���ֽ�   ***
 ***  �� �� �����͵�һ���ֽ� ***
 ***  ����ֵ��Ӧ��λ         ***/
u8 MLX90614_SDA_send(u8 dat)
{
	u8 i = 0,bit_out = 0;
	
	for(i=0;i<8;i++)
	{	  
		if(dat & 0x80)   //�Ǵ����λ��ʼ��
			bit_out=1;    
		else
			bit_out=0;
		MLX90614_SDA_send_bit(bit_out); 
		dat <<= 1;		
	}
	if(MLX90614_SDA_receive_bit())
		return 1;
	else
		return 0;
}



/**����һ��λ**/ 
u8 MLX90614_SDA_receive_bit()
{	
	u8 bit_in = 0;

	MLX90614_SDA_out = 1;
	MLX90614_SCL = 1;
	Delay_us(10);	
	
	if(MLX90614_SDA_in)
		bit_in = 1;
	else
		bit_in = 0;
	
	MLX90614_SCL = 0;
	Delay_us(4);
	
	return bit_in;
}


/**����һ���ֽ�**/
u8 MLX90614_SDA_receive()
{
	u8 i=0,MLX90614_dat=0;
	
	for(i=0;i<8;i++)
	{
		MLX90614_dat <<= 1;
		if(MLX90614_SDA_receive_bit())
			MLX90614_dat |= 0x01;
	}
	MLX90614_SDA_send_bit(0);
	return MLX90614_dat;
}


	
/*��ȡ�¶�ֵ*/
/*
1����ʼ
2������ҪSlave address Tobj1 007h �������¶ȣ�
3���� ��Ӧ��
4������Ҫִ�е����ѡ���RAM��
5��������ʼλ
6�����Ĵ���
7����PEC����
*/

float MLX90614_read_value()
{
	u16 MLX90614_value = 0;
  float MLX90614_t = 0;
	
	MLX90614_SDA_Start();	
  //Slave address ����MLX90614ʱ��ַΪ0x00  ��ַ+wr	
	if(MLX90614_SDA_send(0x00) == 1)    //��Ӧ��  
		return 0;
	//��������
	if(MLX90614_SDA_send(0x07) == 1)
		return 0;
	//-----------
		
	MLX90614_SDA_Start();    //Sr
		
	if(MLX90614_SDA_send(0x01) == 1)
		return 0;
		
	//��Tobj1�Ͱ�λ������Ӧ��
	value_L = MLX90614_SDA_receive();
			
	//��Tobj1�߰�λ������Ӧ��  
	value_H = MLX90614_SDA_receive();

		
	//��PEC�������ݰ�������Ӧ�� 
	PEC = MLX90614_SDA_receive();
		
	MLX90614_SDA_Stop();	
	
	/*���ݵĴ���*/	
	MLX90614_value = value_H;
  MLX90614_value	<<= 8;
	MLX90614_value |= value_L;
	MLX90614_t=MLX90614_value*0.02-272.3;                                                                         5;
	
	return MLX90614_t;	
}


