# include "stm32f10x.h"
# include "oled.h"
# include "stdlib.h"
# include "oledfont.h"
# include "jq8400.h" 

extern int sign1;
extern int sign2;

void Delay(u32 count)
{
  u32 i=0;
  for(;i<count;i++);
}
/**********************************************
//IIC Start
**********************************************/
void OLED_IIC_Start()
{

	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}
/**********************************************
//IIC Stop
**********************************************/
void OLED_IIC_Stop()
{
    OLED_SCLK_Set() ;
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}

void OLED_IIC_Wait_Ack()
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void OLED_Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		m=da;
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
		da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}
}
/**********************************************
// IIC Write Command
**********************************************/
void OLED_Write_IIC_Command(unsigned char IIC_Command)
{
		OLED_IIC_Start();
		OLED_Write_IIC_Byte(0x78);            //Slave address,SA0=0
		OLED_IIC_Wait_Ack();	
		OLED_Write_IIC_Byte(0x00);			//write command
		OLED_IIC_Wait_Ack();	
		OLED_Write_IIC_Byte(IIC_Command); 
		OLED_IIC_Wait_Ack();	
		OLED_IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void OLED_Write_IIC_Data(unsigned char IIC_Data)
{
		OLED_IIC_Start();
		OLED_Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
		OLED_IIC_Wait_Ack();	
		OLED_Write_IIC_Byte(0x40);			//write data
		OLED_IIC_Wait_Ack();	
		OLED_Write_IIC_Byte(IIC_Data);
		OLED_IIC_Wait_Ack();	
		OLED_IIC_Stop();
}
/*
********************************************************************************************************************
*                  void OLED_WR_Byte(u8 dat,u8 cmd)
*
*Description    :??SSD1306??????????????
*Arguments   : dat  ????????????/????    
                           cmd  ????/???????? 0,????????;1,????????;
*Returns     : none
*Notes       : none
********************************************************************************************************************
*/
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
   OLED_Write_IIC_Data(dat);
	}
	else
	{
   OLED_Write_IIC_Command(dat);
	}
}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WR_Byte(fill_Data,1);
			}
	}
}


/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
	unsigned int m;
	for(;Del_50ms>0;Del_50ms--)
		for(m=6245;m>0;m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
	unsigned char j;
	while(Del_1ms--)
	{	
		for(j=0;j<123;j++);
	}
}

//????????
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  
//????OLED????    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC????
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//????OLED????     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC????
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//????????,??????,????????????????!????????????!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //????????????0~7??
		OLED_WR_Byte (0x00,OLED_CMD);      //??????????????????????
		OLED_WR_Byte (0x10,OLED_CMD);      //??????????????????????   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //????????
}
void OLED_On(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //????????????0~7??
		OLED_WR_Byte (0x00,OLED_CMD);      //??????????????????????
		OLED_WR_Byte (0x10,OLED_CMD);      //??????????????????????   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //????????
}
//??????????????????????,????????????
//x:0~127
//y:0~63
//mode:0,????????;1,????????				 
//size:???????? 12/16/24 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//??????????????			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
//m^n????
u32 oled_pow(uint8_t m,uint8_t n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//????2??????
//x,y :????????	 
//len :??????????
//size:????????
//mode:????	0,????????;1,????????
//num:????(0~4294967295);	 		  
void OLED_ShowNumber(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//????????????????
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size)
{
	while (*chr!='\0')
	{		OLED_ShowChar(x,y,*chr,Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			chr++;
	}
}


//==============================================================
//????????LED_ShowCHinese16x16(u8 x,u8 y,u8 num,u8 (*buf)[16]) num????????????buf??????????????????16*16??
//??????????????????????
//??????????????????x,y????y????????0??3??????????????
//????????
//============================================================== 
void OLED_ShowCHinese16x16(uint8_t x,uint8_t y,uint8_t num,uint8_t (*buf)[16])
{      			    
	uint8_t i;
	OLED_Set_Pos(x,y);	
	for(i=0;i<16;i++)
	{
		OLED_WR_Byte(buf[2*num][i],OLED_DATA);
	}	
	OLED_Set_Pos(x,y+1);	

	for(i=0;i<16;i++)
	{	
		OLED_WR_Byte(buf[2*num+1][i],OLED_DATA);
	}			
		
}

/***********??????????????????BMP????128??64??????????(x,y),x??????0??127??y??????????0??7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 
void OLED_Float(unsigned char Y,unsigned char X,double real,unsigned char N) 
{
   unsigned char   i_Count=1;
   unsigned char   n[12]={0};
   long   j=1;  
   int    real_int=0;
   double decimal=0;
   unsigned int   real_decimal=0;
   if(real<0)
	 {
		 real_int=(int)(-real);
	 }
	 else
	 {
		 real_int=(int)real;
   }
	 decimal=real-real_int;
   real_decimal=decimal*1e4;
   while(real_int/10/j!=0)
   {
      j=j*10;i_Count++;  
   } 
   n[0]=(real_int/10000)%10; 
   n[1]=(real_int/1000)%10;
   n[2]=(real_int/100)%10;
   n[3]=(real_int/10)%10;
   n[4]=(real_int/1)%10; 
   n[5]='.';
   n[6]=(real_decimal/1000)%10;
   n[7]=(real_decimal/100)%10;
   n[8]=(real_decimal/10)%10;
   n[9]=real_decimal%10;
   n[6+N]='\0'; 
   for(j=0;j<10;j++) n[j]=n[j]+16+32;
	 if(real<0) 
	 {		 
		 i_Count+=1;
		 n[5-i_Count]='-';
	 }
   n[5]='.';
   n[6+N]='\0';   
   OLED_ShowString(X,Y,&n[5-i_Count],16); 
}

 void OLED_Float2(unsigned char Y,unsigned char X,double real,unsigned char N1,unsigned char N2) 
{
   unsigned char   i_Count=1;
   unsigned char   n[12]={0};
   long   j=1;  
   unsigned int   real_int=0;
   double decimal=0;
   unsigned int   real_decimal=0;
   X=X*8;
   real_int=(int)real;
   //Dis_Num(2,0,real_int,5);
   decimal=real-real_int;
   real_decimal=decimal*1e4;
   //Dis_Num(2,6,real_decimal,4);
   while(real_int/10/j!=0)
   {
      j=j*10;i_Count++;  
   } 
   n[0]=(real_int/10000)%10; 
   n[1]=(real_int/1000)%10;
   n[2]=(real_int/100)%10;
   n[3]=(real_int/10)%10;
 
   n[5]='.';
   n[6]=(real_decimal/1000)%10;
   n[7]=(real_decimal/100)%10;
   n[8]=(real_decimal/10)%10;
   n[9]=real_decimal%10;
   n[6+N2]='\0'; 
   for(j=0;j<10;j++) n[j]=n[j]+16+32;
   n[5]='.';
   n[6+N2]='\0';   
   OLED_ShowString(X,Y,&n[5-N1],12); 
}

void OLED_Num2(unsigned char x,unsigned char y, int number)
{
  unsigned char shi,ge;
	int num =number;
	if(num<0)
	{ 
		num=-num;
		shi=num%100/10;
    ge=num%10;
	  OLED_fuhao_write(x,y,13); 
    OLED_Num_write(x+1,y,shi);
    OLED_Num_write(x+2,y,ge); 
  } 
  else
	{

		shi=num%100/10;
    ge=num%10;
		OLED_fuhao_write(x,y,11);
    OLED_Num_write(x+1,y,shi);
    OLED_Num_write(x+2,y,ge); 
  }
        
}

void OLED_Num3(unsigned char x,unsigned char y,int number)
{
  unsigned char ge,shi,bai;
	int num =number;
	if(num<0)
	{
		    num=-num;
		    OLED_fuhao_write(x,y,13); //????-??
        ge = num %10;
        shi = num/10%10;
        bai = num/100;
        OLED_Num_write(x+3,y,ge);
        OLED_Num_write(x+2,y,shi);
        OLED_Num_write(x+1,y,bai);
	}
	else
	{
       OLED_fuhao_write(x,y,11);
        ge = num %10;
        shi = num/10 %10;
        bai = num/100;
        OLED_Num_write(x+3,y,ge);
        OLED_Num_write(x+2,y,shi);
        OLED_Num_write(x+1,y,bai);
  }
}

void OLED_Num4(unsigned char x,unsigned char y, int number)
{
	unsigned char qian,bai,shi,ge;
	int num =number;
	if(num<0)
	{
		num=-num;
	}
	qian=num/1000;
	bai=num%1000/100;
	shi=num%100/10;
	ge=num%10;

	OLED_Num_write(x,y,qian);
	OLED_Num_write(x+1,y,bai);
	OLED_Num_write(x+2,y,shi);
	OLED_Num_write(x+3,y,ge);
}

void OLED_Num_write(unsigned char x,unsigned char y,unsigned char asc) 
{
	int i=0;
	OLED_Set_Pos(x*6,y);
	for(i=0;i<6;i++)
	{
		 OLED_WR_Byte(F6x8[asc+16][i],OLED_DATA);    
	}
}	
void OLED_fuhao_write(unsigned char x,unsigned char y,unsigned char asc) 
{

	  int i=0;
    OLED_Set_Pos(x*6,y);
    for(i=0;i<6;i++)
    {
       OLED_WR_Byte(F6x8[asc][i],OLED_DATA);         
    }
 
}			

void OLED_Num5(unsigned char x,unsigned char y,unsigned int number)
{
        unsigned char wan,qian,bai,shi,ge;
        wan=number/10000;
	    	qian = number%10000/1000;
        bai=number%1000/100;
        shi=number%100/10;
        ge=number%10;
        OLED_Num_write(x,y,wan);
        OLED_Num_write(x+1,y,qian);
        OLED_Num_write(x+2,y,bai);
        OLED_Num_write(x+3,y,shi);
		    OLED_Num_write(x+4,y,ge);
}

//??????SSD1306					    
void OLED_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //????B????????
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //????????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//????50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //??????GPIOB8,9
 	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);	
	Delay(20000); 
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}  

//????????????????????
void First(void)
{
	OLED_Clear();
	OLED_ShowCHinese16x16(0,0,0,LOGO); //??
	OLED_ShowCHinese16x16(16,0,1,LOGO);//??
	OLED_ShowCHinese16x16(32,0,2,LOGO);//??
	OLED_ShowCHinese16x16(48,0,3,LOGO);//??
	
	OLED_ShowCHinese16x16(0,2,4,LOGO); //??
	OLED_ShowCHinese16x16(16,2,5,LOGO);//??
	OLED_ShowCHinese16x16(32,2,6,LOGO);//??
	OLED_ShowCHinese16x16(48,2,7,LOGO);//??
	
	OLED_ShowCHinese16x16(0,4,10,LOGO); //????????
	OLED_ShowCHinese16x16(16,4,11,LOGO);
	OLED_ShowCHinese16x16(32,4,12,LOGO);
	OLED_ShowCHinese16x16(48,4,13,LOGO);
	
	OLED_ShowCHinese16x16(0,6,21,LOGO); //????????
	OLED_ShowCHinese16x16(16,6,22,LOGO);
	OLED_ShowCHinese16x16(32,6,23,LOGO);
	OLED_ShowCHinese16x16(48,6,24,LOGO);
	
	OLED_ShowString(80,0,"<=",16);
}
void OLED_Mode1()
{
	OLED_ShowCHinese16x16(0,4,14,LOGO); //????????????
	OLED_ShowCHinese16x16(16,4,17,LOGO);
	OLED_ShowCHinese16x16(32,4,17,LOGO);
	OLED_ShowCHinese16x16(48,4,18,LOGO);
	OLED_ShowCHinese16x16(64,4,19,LOGO);
	OLED_ShowCHinese16x16(80,4,20,LOGO);
}
void display(void)  
{	
	OLED_ShowCHinese16x16(0,0,8,LOGO); //??
	OLED_ShowCHinese16x16(16,0,9,LOGO);//??

}
void Reading(void)
{
	OLED_ShowString(0,2,"            ",16);
	OLED_ShowCHinese16x16(0,2,25,LOGO);//??????...
	OLED_ShowCHinese16x16(16,2,26,LOGO);
	OLED_ShowCHinese16x16(32,2,27,LOGO);
}
void Finish(void)
{
	OLED_ShowString(0,2,"          ",16);
	OLED_ShowCHinese16x16(0,2,28,LOGO);
	OLED_ShowCHinese16x16(16,2,29,LOGO);
}
void OLED_Mode2(void)
{
	OLED_ShowCHinese16x16(0,2,4,LOGO);  //??????
	OLED_ShowCHinese16x16(16,2,5,LOGO);
	OLED_ShowCHinese16x16(32,2,27,LOGO);
}
void Fire(void)
{
		OLED_ShowCHinese16x16(0,6,23,LOGO);//??????????
		OLED_ShowCHinese16x16(16,6,24,LOGO);
		OLED_ShowCHinese16x16(32,6,32,LOGO);
		OLED_ShowCHinese16x16(48,6,33,LOGO);
		OLED_ShowString(64,6,"!",16);
}

void His(void)
{
		//??????
		OLED_ShowCHinese16x16(0,0,34,LOGO);
		OLED_ShowCHinese16x16(16,0,35,LOGO);
		OLED_ShowCHinese16x16(32,0,36,LOGO);
		//??????
		OLED_ShowCHinese16x16(0,2,34,LOGO);
		OLED_ShowCHinese16x16(16,2,37,LOGO);
		OLED_ShowCHinese16x16(32,2,36,LOGO);
		//??????
		OLED_ShowCHinese16x16(0,4,34,LOGO);
		OLED_ShowCHinese16x16(16,4,38,LOGO);
		OLED_ShowCHinese16x16(32,4,36,LOGO);
		//??????
		OLED_ShowCHinese16x16(0,6,34,LOGO);
		OLED_ShowCHinese16x16(16,6,39,LOGO);
		OLED_ShowCHinese16x16(32,6,36,LOGO);
}
//??????????????
void People(int signx)
{
	if(signx==0)
	{
		OLED_ShowCHinese16x16(112,6,40,LOGO);
			playnum(20);
	}
	if(signx==1)
	{
		OLED_ShowCHinese16x16(112,6,41,LOGO);
			playnum(19);
	}
}














