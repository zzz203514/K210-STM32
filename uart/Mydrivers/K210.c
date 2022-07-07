#include "K210.h"
#include "stdio.h"
static uint8_t color=0,qrcode=0,ColorXian;   //1为红色，2为绿色，3为蓝色
static uint8_t colorShu[3] ={0};
static uint8_t block,TrueOne;
//宝物三个地点
void BaoWuLocation(uint8_t qrcode)    
{
	uint8_t lo1=0,lo2=0,lo3=0;
	lo1=qrcode/100;
	lo2=(qrcode%100)/10;
	lo3=qrcode%10;
}
//com_data是K210传的数据，共8个字节
void K210_Color_Data(uint8_t com_data[])  
{
		uint8_t i,k=0;
		static uint8_t RxCounter1=0;//计数
		static uint16_t RxBuffer1[5]={0};
		static uint8_t RxState1 = 0;	
		static uint8_t RxFlag1 = 0;
				if(RxState1==0&&com_data[0]==0x2C)
				{
					RxState1=1;
					RxBuffer1[RxCounter1++]=com_data[0];
					//printf("%d\r\n",com_data);
				}
				if(RxState1==1&&com_data[1]==0x12)  //0x12帧头        //如果用else if，就不会进入下一个语句
				{
					RxState1=2;
					RxBuffer1[RxCounter1++]=com_data[1];
					//printf("%d\r\n",com_data);
				}
				 if(RxState1==2)     //这里的else会让除第一位数据失效
				{
          for(;RxCounter1<5;RxCounter1++)
						RxBuffer1[RxCounter1]=com_data[RxCounter1];
					if(RxCounter1>=5||com_data[RxCounter1-1]== 0x5B)       //RxBuffer1接受满了,接收数据结束
					{
						RxState1=3;
						RxFlag1=1;       //接收完成标志
            color=(RxBuffer1[3]<<8)|RxBuffer1[2];
						//Cy=(RxBuffer1[5]<<8)|RxBuffer1[4];
						//color=(RxBuffer1[7]<<8)|RxBuffer1[6];     
           //printf("%d\r",Cx);
           //printf("%d\r",Cy);
						printf("%d\r\n",color);

					}		
			}
		
				if(RxState1==3)		//检测是否接受到结束标志
				{
						if(RxBuffer1[RxCounter1-1] == 0x5B)
						{
									RxFlag1 = 0;
									RxCounter1 = 0;       //重置这些值，给下一次中断
									RxState1 = 0;
							
									for(int j=0;j<=2;j++)
									{
										if((color-1)==j)
											colorShu[j]++;  //识别到多少次同一种颜色
										printf("%d\r\n",colorShu[j]);
									}
									
									for(int j=0;j<=2;j++)
									{
										if(k==0&&colorShu[j]>2)
										{
											ColorXian=j+1;       //将学习颜色存储在colorYuan中
											k=123;                   //k值用于判断是否学习了颜色
											printf("%d\r\n",k);    //这里应加个延时，用状态机
										}
										else if(k==1&&colorShu[j]>2)
										{
											if(color==ColorXian)
												TrueOne=1;  //需先设定为0
										}
									}
					  }					
				else   //接收错误
						{
									RxState1 = 0;
									RxCounter1 =0;
									for(i=0;i<5;i++)
									{
											RxBuffer1[i]=0x00;      //将存放数据数组清零
									}
						}
				} 
	
				else   //接收异常
				{
						RxState1 = 0;
						RxCounter1=0;
						for(i=0;i<5;i++)
						{
								RxBuffer1[i]=0x00;      //将存放数据数组清零
						}
				}
}
//处理二维码和识别白板有无
void	K210_Qrcode_Data(uint8_t qr_data[])
{
		uint8_t i;
		static uint8_t RxCounter2=0;//计数
		static uint16_t RxBuffer2[5]={0};
		static uint8_t RxState2 = 0;	
		static uint8_t RxFlag2 = 0;
		if(RxState2==0&&qr_data[0]==0x2C)  //0x2c帧头
				{
					RxState2=1;
					RxBuffer2[RxCounter2++]=qr_data[0];
				}

		if(RxState2==1&&qr_data[1]==0x12)  //0x12帧头
				{
					RxState2=2;
					RxBuffer2[RxCounter2++]=qr_data[1];
				}
		if(RxState2==2)
				{
					for(;RxCounter2<=4;RxCounter2++)
						RxBuffer2[RxCounter2]=qr_data[RxCounter2];
					if(RxCounter2>=5||qr_data[RxCounter2-1] == 0x5B)
					{
						RxState2=3;
						RxFlag2=1;
						qrcode=(RxBuffer2[3]<<8)|RxBuffer2[2];
						if(qrcode==0)  //前面有白板
						{
							block=1; 
							printf("block is %d\r\n",block);							
							//whiteS++;
							//Forbid(whiteS);
						}
						else
						{
							printf("%d\r\n",qrcode);
							BaoWuLocation(qrcode);     //将宝物地点存储 
						}
					}
				}
			if(RxState2==3)		//检测是否接受到结束标志
				{
						if(RxBuffer2[RxCounter2 -1] == 0x5B)
						{
									
									RxFlag2 = 0;
									RxCounter2 = 0;
									RxState2 = 0;
									if(block==1)
										Forbid();
						}
						else   //接收错误
						{
									RxState2 = 0;
									RxCounter2 =0;
									for(i=0;i<5;i++)
									{
											RxBuffer2[i]=0x00;      //将存放数据数组清零
									}
						}
				} 
	
				else   //接收异常
				{
						RxState2 = 0;
						RxCounter2=0;
						for(i=0;i<5;i++)
						{
								RxBuffer2[i]=0x00;      //将存放数据数组清零
						}
				}
}
//判断是否有白色障碍物
void  Forbid(void)
{
	uint8_t k=0;
	while(k<100)
	{
		k++;
		if(k>=100)
			block=0;
	}
	printf("block is %d\r\n",block);
	printf("进入了Forbid\r\n");
}
