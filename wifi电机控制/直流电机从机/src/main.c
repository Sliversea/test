#include "config.h"

uchar 	uart1_wr;		//写指针
uchar 	uart1_rd;		//读指针
uchar 	xdata RX1_Buffer[BUF_LENTH];	//接收缓冲
uchar   xdata mbus_buffer[255];
uchar   xdata mbus_Sendbuf[255];
uchar   xdata Crc_buf[2];  //声明存储CRC校验值的高8位及低8位的缓存

uint  Crc_return_data;  //声明CRC校验值
//***************更多功率参数设置可详细参考DATACC1100英文文档中第48-49页的参数表******************
//uchar PaTabel[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   功率最小
uchar PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
//uchar PaTabel[8] = {0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0};   //10dBm     功率最大
//*****************************************************************************************

uchar devid;
uchar leng =0;
uchar leng_w = 0;
uchar flag;
uchar flag1 = 1;
uchar flag2;
uchar flag3;
uchar i;
uint j;
uint T0_flag = 0;


uchar TxBuf[20]={"www.frotech.com"};	 // 8字节, 如果需要更长的数据包,请正确设置
extern unsigned char xdata Sensor_Data[5];
extern unsigned char xdata BUF[8];  //接收数据缓存区

extern float temp_disp;   //温度显值
extern float humi_disp;   //湿度显值
extern uchar temp_H;        //全局应答变量
extern uchar temp_L;        //全局应答变量

unsigned char code F_Rotation[4]={0x02,0x04,0x08,0x10};//正转表格
unsigned char code B_Rotation[4]={0x10,0x08,0x04,0x02};//反转表格
unsigned char temp1,temp2,temp3 ;
	
bit		B_TI;

void zhudong();
void Read_Sense(void);
void Write_Sense(void);

void Time0_init(void);

void Delay(unsigned int i);



void	main(void)
{
    
	Sensor_SDA = 1;
	GF3658_init();
	uart1_init();

	Init_ADXL345(); //初始化ADXL345
	devid=Single_Read_ADXL345(0X00);
	Uart1_TxByte(devid);

	ADC_CONTR = ADC_360T | ADC_ON;

	EEPROM_read_n(0x0000,mbus_buffer,255);
	Uart1_String("www.1.com");


	//Time0_init();


 	mbus_buffer[0x50] = 0x1C;
	mbus_buffer[0x54] = 0x0E;
	mbus_buffer[0x55] = 0x08;

	while(flag1)  //是否初始化标志
	{
	     if(mbus_buffer[0x03] == 1)		 //RX1 :05 31 (00 05 01 01--配置信息mbus_buff) mbus_buffer: 05 03 00 28 00 01 05 86	  
		 {
		     flag1 = 0;
		 }
	     if(flag == 1) //串口是否有数据
		 {
		     delay_ms(1);									 
		     uart1_wr = 0;
		     flag = 0;
			 if((RX1_Buffer[0] == 0xFF) && (RX1_Buffer[1] == 0x30))		   //RX1_Buffer:FF 30 00 05 01 01
			 {
			     for(i = 0; i < 4; i++)
				 {
			         mbus_buffer[i] = RX1_Buffer[i+2];
				     EEPROM_write_byte(0x0000, mbus_buffer, 255, i, RX1_Buffer[i+2]);
					 mbus_Sendbuf[i + 2] = mbus_buffer[i];
				 }
				 mbus_Sendbuf[0] = mbus_buffer[1];		 //mbus_Sendbuf:05 30 00 05 01 01 
				 mbus_Sendbuf[1] = 0x30;
				 for(i = 0; i < 6; i++)
				 {
				      Uart1_TxByte( mbus_Sendbuf[i]);
				 }
			 } 
			 if((RX1_Buffer[0] == 0xFF) && (RX1_Buffer[1] == 0x31))	  //发送FF 31 mbus_Sendbuf：05 31 (00 05 01 01--配置信息mbus_buffer） 
			 {
			    mbus_Sendbuf[0] = mbus_buffer[1];		   //mbus_buffer:
				mbus_Sendbuf[1] = 0x31;
				for(i = 0; i < 4; i++)
				{
				     mbus_Sendbuf[i + 2] = mbus_buffer[i];	//mbus_Sendbuf:05 03 00 28 00 01 05 86	  mbus_buffer:05 03 02 00 00 49 84
				}
			    for(i = 0; i < 6; i++)
				{
					 Uart1_TxByte( mbus_Sendbuf[i]);
				} 
			 }
			 
		
		 }
		  							 
	}
	 if(mbus_buffer[0x02] == 0x01)
	 {
		 CpuInit();
	     POWER_UP_RESET_CC1100();
	     halRfWriteRfSettings();
	     halSpiWriteBurstReg(CCxxx0_PATABLE, PaTabel, 8);	     
	 }


		 Uart1_String("www.2.com");

	while(1)
	{
		  

	    /**********本部分在扫描数码管的时候开启**********/

	 	/*GF3658_data(mbus_buffer[0x50]);
		GF3658_map(mbus_buffer[0x54]);
		GF3658_map(mbus_buffer[0x55]);
	


		GF3658_map(9);
		GF3658_map(10);
		GF3658_map(11);
		GF3658_map(12);*/	
		/************************************************/

		/*if((0 <= T0_flag) && (T0_flag <= 800))
		{
		     COM4 = 1;
	         set1 = 0;
			 delay_GF();
			 COM4 = 0;
	         set1 = 1;
		}
        if((801 <= T0_flag) && (T0_flag <= 1600))
		{
		     COM4 = 1;
	         set1 = 0; 
			 set2 = 0;    
	         delay_GF();
			 COM4 = 0;
	         set1 = 1; 
			 set2 = 1; 
	         
		}
		if((1601 <= T0_flag) && (T0_flag <= 2400))
		{
		     COM4 = 1;
	         set1 = 0; 
			 set2 = 0; 
			 set3 = 0;   
	         delay_GF();
			 COM4 = 0;
	         set1 = 1; 
			 set2 = 1;
			 set3 = 1;
		}
		if((2401 <= T0_flag) && (T0_flag <= 3200))
		{
		     COM4 = 1;
	         set1 = 0; 
			 set2 = 0; 
			 set3 = 0;  
			 set4 = 0; 
	         delay_GF();
			 COM4 = 0;
	         set1 = 1; 
			 set2 = 1;
			 set3 = 1;
			 set4 = 1;
		}
		if( TF0 == 1)
		{
		    TF0 == 0;
		    TH0 = 0xfC;
			TL0 = 0x67;
			T0_flag++;
			if(T0_flag == 3700)
	        {
	           T0_flag = 0;
	        }
		}*/
		leng=63;
	    if(mbus_buffer[0x02] == 0x01)
		{
		    
		    leng=63;
		    if(halRfReceivePacket(RX1_Buffer,&leng))
			{
			     
			     delay_ms(50);
			     if(RX1_Buffer[0] == mbus_buffer[0x01])
		         { 
		           if(RX1_Buffer[1] == 0x03)
		           {
		       
                       Read_Sense();
		           }
				   if(RX1_Buffer[1] == 0x10)
		           {
		       			Uart1_TxByte('%');
                        Write_Sense();
		           }
		         }

				 if((RX1_Buffer[0] == 0xFF) && (RX1_Buffer[1] == 0x30))
			     {
			         for(i = 0; i < 4; i++)
				     {
			             mbus_buffer[i] = RX1_Buffer[i+2];
						 mbus_Sendbuf[i+2]= mbus_buffer[i];
				         EEPROM_write_byte(0x0000, mbus_buffer, 255, i, RX1_Buffer[i+2]); 
				     }
					 mbus_Sendbuf[1]= 0x30;
			         halRfSendPacket(mbus_Sendbuf, 6);
			        
			     }

				 if((RX1_Buffer[0] == 0xFF) && (RX1_Buffer[1] == 0x31))
		         {
				    for(i = 0; i < 4; i++)
				    {
						 mbus_Sendbuf[i+2]= mbus_buffer[i];   
				    }
					mbus_Sendbuf[1]= 0x31;
					//delay_ms(((mbus_buffer[0] & 0xFF))*10);
			        halRfSendPacket(mbus_Sendbuf, 6);
		         }

			}

		    
		}

	    if(flag == 1)
		{
		   delay_ms(3);									 
		   uart1_wr = 0;
		   flag = 0;
		   if(RX1_Buffer[0] == mbus_buffer[0x01])
		   { 
		       if(RX1_Buffer[1] == 0x03)
		       {
		       
                   Read_Sense();
				   
		       }
			   if(RX1_Buffer[1] == 0x10)
			   {
			       Uart1_String("www.3.com");
			       Write_Sense();

			   }
		   }
		   if((RX1_Buffer[0] == 0xFF) && (RX1_Buffer[1] == 0x30))
		   {
			     for(i = 0; i < 4; i++)
				 {
			         mbus_buffer[i] = RX1_Buffer[i+2];
				     EEPROM_write_byte(0x0000, mbus_buffer, 255, i, RX1_Buffer[i+2]); 
					 mbus_Sendbuf[i + 2] = mbus_buffer[i];
				 }
				 mbus_Sendbuf[0] = mbus_buffer[1];
				 mbus_Sendbuf[1] = 0x30;
				 for(i = 0; i < 6; i++)
				 {
				      Uart1_TxByte( mbus_Sendbuf[i]);
				 }
		  }
		   if((RX1_Buffer[0] == 0xFF) && (RX1_Buffer[1] == 0x31))
		   {
			    mbus_Sendbuf[0] = mbus_buffer[1];
				mbus_Sendbuf[1] = 0x31;
				for(i = 0; i < 4; i++)
				{
				     mbus_Sendbuf[i + 2] = mbus_buffer[i];
				}
			    for(i = 0; i < 6; i++)
				{
					 Uart1_TxByte( mbus_Sendbuf[i]);
				} 
		   }

		}

	}
}

void Write_Sense(void)
{
	
    leng_w = (RX1_Buffer[6] + 7);
    Crc_return_data=cal_crc(RX1_Buffer, leng_w);  //取得将要发送数据的CRC值
	Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位

	
	if((RX1_Buffer[leng_w] == Crc_buf[1]) && (RX1_Buffer[1 + leng_w] == Crc_buf[0]))
	{	
		
		     //LED显示模块
	         if(RX1_Buffer[0x03] == 0x50)
	         {
	   
                 mbus_buffer[0x50] = RX1_Buffer[0x0A];
		         mbus_buffer[0x54] = RX1_Buffer[0x0B];
		         mbus_buffer[0x55] = RX1_Buffer[0x0C];
				 if(mbus_buffer[0x02] == 0x01)
			     {
			         halRfSendPacket(RX1_Buffer, 15);
			     }
			     else
			     {
			         for(i = 0; i < 15; i++)
		             {             
				         Uart1_TxByte(RX1_Buffer[i]);	
		             }
			     }

	         }

			 if(RX1_Buffer[0x03] == 0x4A)
	         {
	   			  temp1 = P1;
                  //temp1 = P1^5;	               
				  //temp2 = P1^6;
				  //temp3 = P1^7;

                  if(RX1_Buffer[7] == 0x01)
                  {
                       for(j = 0; j < RX1_Buffer[8]; j++)
                       {
                           for(i=0;i<4;i++)      //4相
                           {
						      
	                          P1=F_Rotation[i];;  //输出对应的相 可以自行换成反转表格
	                          Delay(1500);
							  Delay(500);
	                      }
                       }
                  }
                  
            
                  
                  if(RX1_Buffer[7] == 0x02)
                  {
                       for(j = 0; j < RX1_Buffer[8]; j++)
                       {
                           for(i=0;i<4;i++)      //4相
                           {
	                          P1=B_Rotation[i];  //输出对应的相 可以自行换成反转表格
	                          Delay(1500);
							  Delay(500);                       
                            
	                       }
                       }
                  }

				  P1 = temp1;
				  //P1^5 = temp1;	               
				  //P1^6 = temp2;
				  //P1^7 = temp3;
				 

	         }

		
		   //直流电机模块
	      if(RX1_Buffer[0x03] == 0x48)
	      {
		   	  	 Uart1_String("www.4.com");
	           P1M0 |= 0x02;
	          if(RX1_Buffer[0x08] == 0x01)
		     {
		         Uart1_String("www.5.com");
				shuchu = 0;
				
		     }
		     else
		    {
			 Uart1_String("www.6.com");
		        shuchu = 1;
		    }
	}







		
	}
}


void zhudong(void)
{
    if(mbus_buffer[0x01] == 1)
	{
	    if(mbus_buffer[0x04] > 0xFA)
		{
		   mbus_buffer[0x04] = 0x05; 
		}
		
	    while(1)
		{
		    delay_ms(mbus_buffer[0x04]);
			delay_ms(mbus_buffer[0x04]);
		    
		    Read_Sense();
		  



		}
	}
}

void Read_Sense(void)
{
    Crc_return_data=cal_crc(RX1_Buffer, 6);  //取得将要发送数据的CRC值
	Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
	if((RX1_Buffer[6] == Crc_buf[1]) && (RX1_Buffer[7] == Crc_buf[0]))
	{
	    //温湿度传感器
    if(RX1_Buffer[0x03] == 0x14)
	{		   
			   Read_Sensor();            //采集温湿度值
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2);  
			   for(i = 0; i < 4; i++)
		       {             
				    mbus_Sendbuf[3 + i] = Sensor_Data[i];	
		       }
			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;
			   
			   Crc_return_data=cal_crc(mbus_Sendbuf, 7);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[7]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[8]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 9);
			   }
			   else
			   {
			       for(i = 0; i < 9; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }
			   }
	}
			//加速度传感器
	if(RX1_Buffer[0x03] == 0x18)
	{			//
			   if(devid == 0xE5) //读出的数据为0XE5,表示正确读出器件的ID号	
		       {
			       mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2);
		           Multiple_Read_ADXL345();       	//连续读出数据，存储在BUF中
		           for(i=0; i<6; i++)
		           {
		              	 mbus_Sendbuf[i+3] = BUF[i];
		           }
				   mbus_Sendbuf[0] = mbus_buffer[1];  
			       mbus_Sendbuf[1] = 0x03;
			         
			   
			       Crc_return_data=cal_crc(mbus_Sendbuf, 10);  //取得将要发送数据的CRC值
	               Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	               Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			       mbus_Sendbuf[9]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			       mbus_Sendbuf[10]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			       if(mbus_buffer[0x02] == 0x01)
			       { 
			           halRfSendPacket(mbus_Sendbuf, 11);
			       }
			       else
			      {
			           for(i = 0; i < 11; i++)
		               {             
				            Uart1_TxByte(mbus_Sendbuf[i]);	
		               }	
			      }
		       }    
			  
	}
			//磁控检测传感器
	if(RX1_Buffer[0x03] == 0x28)	  //RX1_Buffer：05 03 00 28 00 01 05 86  mbus_Sendbuf：05 03 02 00 00 49 84 ;
	{
			   mbus_Sendbuf[3] = 0x00;
			   mbus_Sendbuf[4] = 0x00;
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2);	// 00*16+01*2=2
			   if(P1_0 == 0)
			   {
			       mbus_Sendbuf[4] = 0x01;		//mbus_Sendbu:05 03 02 00 01 88 44
			   }
			   mbus_Sendbuf[0] = mbus_buffer[1];  	  //mbus_buffer:05 31 00 05 01 01 	配置信息
			   mbus_Sendbuf[1] = 0x03;
			     
			
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01) //00 05 01 01
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}
			//光敏传感器
	if(RX1_Buffer[0x03] == 0x2A)	//09 03 00 2A 00 01 a4 8a 	  mbus_Sendbuf：09 03 02 02 E7 18 AF 
	{

			   P1ASF = (1 << ADC_CH1);	//STC12C5A16S2系列模拟输入(AD)选择ADC1(P1.1)
			   j = adc10_start(1);	//(P1.1)ADC1转换
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2);  //  00*4+01*2=2
			   mbus_Sendbuf[3] = ((j >> 8) & 0xFF);
			   mbus_Sendbuf[4] = (j & 0xFF);

			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;
			     
			  
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}
			//红外对射传感器
	if(RX1_Buffer[0x03] == 0x2C)
	{

			   P1ASF = (1 << ADC_CH0);	//STC12C5A16S2系列模拟输入(AD)选择ADC1(P1.1)
			   j = adc10_start(0);	//(P1.1)ADC1转换
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2); 
			   mbus_Sendbuf[3] = ((j >> 8) & 0xFF);
			   mbus_Sendbuf[4] = (j & 0xFF);

			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;
			    
			  
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}
			//结露传感器
	if(RX1_Buffer[0x03] == 0x2E)
	{

			   P1ASF = (1 << ADC_CH1);	//STC12C5A16S2系列模拟输入(AD)选择ADC1(P1.1)
			   j = adc10_start(1);	//(P1.1)ADC1转换
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2);
			   mbus_Sendbuf[3] = ((j >> 8) & 0xFF);
			   mbus_Sendbuf[4] = (j & 0xFF);

			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;
			      
		
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}
			//酒精传感器
	if(RX1_Buffer[0x03] == 0x30)
	{

			   P1ASF = (1 << ADC_CH1);	//STC12C5A16S2系列模拟输入(AD)选择ADC1(P1.1)
			   j = adc10_start(1);	//(P1.1)ADC1转换
			   
			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;

			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2); 
			   mbus_Sendbuf[3] = ((j >> 8) & 0xFF);
			   mbus_Sendbuf[4] = (j & 0xFF);
			
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}
			//振动传感器
	if(RX1_Buffer[0x03] == 0x32)
	{
	           flag3 = 1;
			   while(flag3)
			   {
			       if(P1_0 == 0)
			       {
				       flag3 = 0;
			           mbus_Sendbuf[0] = mbus_buffer[1];  
			           mbus_Sendbuf[1] = 0x03;
			           mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2);   
			       
					   mbus_Sendbuf[3] = 0x00;
					   mbus_Sendbuf[4] = 0x01;  
			           Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	                   Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	                   Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			           mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			           mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			           if(mbus_buffer[0x02] == 0x01)
			           {
			               halRfSendPacket(mbus_Sendbuf, 7);
			           }
			           else
			           {
			           for(i = 0; i < 7; i++)
		               {             
				            Uart1_TxByte(mbus_Sendbuf[i]);	
		               }	
			           }
			       }
			  	  
				  
			   }
	}
		
			//烟雾传感器
	if(RX1_Buffer[0x03] == 0x34)
	{

			   P1ASF = (1 << ADC_CH1);	//STC12C5A16S2系列模拟输入(AD)选择ADC1(P1.1)
			   j = adc10_start(1);	//(P1.1)ADC1转换
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2); 
			   mbus_Sendbuf[3] = ((j >> 8) & 0xFF);
			   mbus_Sendbuf[4] = (j & 0xFF);

			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;
			     
			 
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}
	//红外反射传感器
	if(RX1_Buffer[0x03] == 0x36)
	{
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2); 
			   mbus_Sendbuf[3] = 0x00;
			   mbus_Sendbuf[4] = 0x00;
			   if(P1_0 == 0)
			   {
			       mbus_Sendbuf[4] = 0x01;
			   }
			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;
			    
			
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}
	//火焰传感器
	if(RX1_Buffer[0x03] == 0x38)
	{
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2); 
			   mbus_Sendbuf[3] = 0x00;
			   mbus_Sendbuf[4] = 0x00;
			   if(P1_0 == 0)
			   {
			       mbus_Sendbuf[4] = 0x01;
			   }
			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;
			   
			
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}
	//声音传感器
	if(RX1_Buffer[0x03] == 0x3A)
	{
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2);
			   flag3 = 1;
			   while(flag3)
			   {
			       
			       if(P1_0 == 0)
			       {
				       flag3 = 0;
			           mbus_Sendbuf[0] = mbus_buffer[1];  
			           mbus_Sendbuf[1] = 0x03;
			              
			       
					   mbus_Sendbuf[3] = 0x00;
					   mbus_Sendbuf[4] = 0x01;  
			           Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	                   Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	                   Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			           mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			           mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			           if(mbus_buffer[0x02] == 0x01)
			           {
			               halRfSendPacket(mbus_Sendbuf, 7);
			           }
			           else
			           {
			           for(i = 0; i < 7; i++)
		               {             
				            Uart1_TxByte(mbus_Sendbuf[i]);	
		               }	
			           }
			       }
			  	  
				  
			   }
	}

    //人体感应传感器


	//水流量传感器


	//雨滴检测传感器
	if(RX1_Buffer[0x03] == 0x40)
	{

			   P1ASF = (1 << ADC_CH1);	//STC12C5A16S2系列模拟输入(AD)选择ADC1(P1.1)
			   j = adc10_start(1);	//(P1.1)ADC1转换
			   mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2);
			   mbus_Sendbuf[3] = ((j >> 8) & 0xFF);
			   mbus_Sendbuf[4] = (j & 0xFF);

			   mbus_Sendbuf[0] = mbus_buffer[1];  
			   mbus_Sendbuf[1] = 0x03;
			      
		
			   Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	           Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	           Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
			   mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
			   mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
			   if(mbus_buffer[0x02] == 0x01)
			   {
			       halRfSendPacket(mbus_Sendbuf, 7);
			   }
			   else
			   {
			       for(i = 0; i < 7; i++)
		           {             
				        Uart1_TxByte(mbus_Sendbuf[i]);	
		           }	
			   }
	}

	//压力传感器


	//超声波测距传感器


	

	//步进电机模块
	

	//SHT10温湿度模块
	if(RX1_Buffer[0x03] == 0x4C)
	{
	     deal_whole();
	     
         mbus_Sendbuf[2] = (((RX1_Buffer[0x04] * 16) + RX1_Buffer[0x05]) * 2); 
	     mbus_Sendbuf[3] = temp_disp;
		 mbus_Sendbuf[4] = humi_disp;
		 mbus_Sendbuf[0] = mbus_buffer[1];  
		 mbus_Sendbuf[1] = 0x03;
			   
			
		 Crc_return_data=cal_crc(mbus_Sendbuf, 5);  //取得将要发送数据的CRC值
	     Crc_buf[0]=((Crc_return_data >> 8)&0xff);  //取得CRC值的高8位
	     Crc_buf[1]=(Crc_return_data & 0xff);   //取得CRC值的低8位
		 mbus_Sendbuf[5]=Crc_buf[1];  //CRC值低8位赋值给将要发送的数据的倒数第二个字节
		 mbus_Sendbuf[6]=Crc_buf[0];  //CRC值高8位赋值给将要发送的数据的最后一个
		 if(mbus_buffer[0x02] == 0x01)
		 {
			   halRfSendPacket(mbus_Sendbuf, 7);
		 }
		 else
		 {
			   for(i = 0; i < 7; i++)
		       {             
				    Uart1_TxByte(mbus_Sendbuf[i]);	
		       }	
		 }    
	}


   



	}
	
}






void	uart1_init(void)
{
	PCON |= 0x80;		//UART0 Double Rate Enable
	SCON = 0x50;		//UART0 set as 10bit , UART0 RX enable
	AUXR |=  0x01;		//UART0 使用BRT
	AUXR |=  0x04;		//BRT set as 1T mode
	BRT = BRT_Reload;
	AUXR |=  0x10;		//start BRT	

	ES  = 1;
	EA = 1;
}

void Uart1_TxByte(unsigned char dat)
{
    B_TI = 0;
	SBUF = dat;
	while(!B_TI);
	B_TI = 0;
}

void Uart1_String(unsigned char code *puts)
{
    for(; *puts != 0; puts++)
	{
        Uart1_TxByte(*puts);

	}
}


/**********************************************/
void UART1_RCV (void) interrupt 4
{
	if(RI)
	{
		RI = 0;
		RX1_Buffer[uart1_wr++] = SBUF;
		flag = 1;
	}

	if(TI)
	{
		TI = 0;
		B_TI = 1;
	}
}

void Time0_init(void)
{
    AUXR = 0x00;
	TMOD = 0x01;
	TL0  = 0x67;
	TH0  = 0xFC;		
	TR0  = 1;
	//ET0  = 1;
	EA   = 1;
}

/*void T0 (void) interrupt 1
{
    TL0  = 0x67;
	TH0  = 0xfC;
    T0_flag++;
	if(T0_flag == 3700)
	{
	    T0_flag = 0;
	}

}*/

void delay_ms(unsigned char ms)
{
    unsigned int i;
	do{
	    i = MAIN_Fosc /1400;
		while(--i);
	}while(--ms);
}

void Delay(unsigned int i)//延时
{
 while(--i);              
}




