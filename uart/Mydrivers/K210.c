#include "K210.h"
#include "stdio.h"
static uint8_t color=0,qrcode=0,ColorXian;   //1Ϊ��ɫ��2Ϊ��ɫ��3Ϊ��ɫ
static uint8_t colorShu[3] ={0};
static uint8_t block,TrueOne;
//���������ص�
void BaoWuLocation(uint8_t qrcode)    
{
	uint8_t lo1=0,lo2=0,lo3=0;
	lo1=qrcode/100;
	lo2=(qrcode%100)/10;
	lo3=qrcode%10;
}
//com_data��K210�������ݣ���8���ֽ�
void K210_Color_Data(uint8_t com_data[])  
{
		uint8_t i,k=0;
		static uint8_t RxCounter1=0;//����
		static uint16_t RxBuffer1[5]={0};
		static uint8_t RxState1 = 0;	
		static uint8_t RxFlag1 = 0;
				if(RxState1==0&&com_data[0]==0x2C)
				{
					RxState1=1;
					RxBuffer1[RxCounter1++]=com_data[0];
					//printf("%d\r\n",com_data);
				}
				if(RxState1==1&&com_data[1]==0x12)  //0x12֡ͷ        //�����else if���Ͳ��������һ�����
				{
					RxState1=2;
					RxBuffer1[RxCounter1++]=com_data[1];
					//printf("%d\r\n",com_data);
				}
				 if(RxState1==2)     //�����else���ó���һλ����ʧЧ
				{
          for(;RxCounter1<5;RxCounter1++)
						RxBuffer1[RxCounter1]=com_data[RxCounter1];
					if(RxCounter1>=5||com_data[RxCounter1-1]== 0x5B)       //RxBuffer1��������,�������ݽ���
					{
						RxState1=3;
						RxFlag1=1;       //������ɱ�־
            color=(RxBuffer1[3]<<8)|RxBuffer1[2];
						//Cy=(RxBuffer1[5]<<8)|RxBuffer1[4];
						//color=(RxBuffer1[7]<<8)|RxBuffer1[6];     
           //printf("%d\r",Cx);
           //printf("%d\r",Cy);
						printf("%d\r\n",color);

					}		
			}
		
				if(RxState1==3)		//����Ƿ���ܵ�������־
				{
						if(RxBuffer1[RxCounter1-1] == 0x5B)
						{
									RxFlag1 = 0;
									RxCounter1 = 0;       //������Щֵ������һ���ж�
									RxState1 = 0;
							
									for(int j=0;j<=2;j++)
									{
										if((color-1)==j)
											colorShu[j]++;  //ʶ�𵽶��ٴ�ͬһ����ɫ
										printf("%d\r\n",colorShu[j]);
									}
									
									for(int j=0;j<=2;j++)
									{
										if(k==0&&colorShu[j]>2)
										{
											ColorXian=j+1;       //��ѧϰ��ɫ�洢��colorYuan��
											k=123;                   //kֵ�����ж��Ƿ�ѧϰ����ɫ
											printf("%d\r\n",k);    //����Ӧ�Ӹ���ʱ����״̬��
										}
										else if(k==1&&colorShu[j]>2)
										{
											if(color==ColorXian)
												TrueOne=1;  //�����趨Ϊ0
										}
									}
					  }					
				else   //���մ���
						{
									RxState1 = 0;
									RxCounter1 =0;
									for(i=0;i<5;i++)
									{
											RxBuffer1[i]=0x00;      //�����������������
									}
						}
				} 
	
				else   //�����쳣
				{
						RxState1 = 0;
						RxCounter1=0;
						for(i=0;i<5;i++)
						{
								RxBuffer1[i]=0x00;      //�����������������
						}
				}
}
//�����ά���ʶ��װ�����
void	K210_Qrcode_Data(uint8_t qr_data[])
{
		uint8_t i;
		static uint8_t RxCounter2=0;//����
		static uint16_t RxBuffer2[5]={0};
		static uint8_t RxState2 = 0;	
		static uint8_t RxFlag2 = 0;
		if(RxState2==0&&qr_data[0]==0x2C)  //0x2c֡ͷ
				{
					RxState2=1;
					RxBuffer2[RxCounter2++]=qr_data[0];
				}

		if(RxState2==1&&qr_data[1]==0x12)  //0x12֡ͷ
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
						if(qrcode==0)  //ǰ���аװ�
						{
							block=1; 
							printf("block is %d\r\n",block);							
							//whiteS++;
							//Forbid(whiteS);
						}
						else
						{
							printf("%d\r\n",qrcode);
							BaoWuLocation(qrcode);     //������ص�洢 
						}
					}
				}
			if(RxState2==3)		//����Ƿ���ܵ�������־
				{
						if(RxBuffer2[RxCounter2 -1] == 0x5B)
						{
									
									RxFlag2 = 0;
									RxCounter2 = 0;
									RxState2 = 0;
									if(block==1)
										Forbid();
						}
						else   //���մ���
						{
									RxState2 = 0;
									RxCounter2 =0;
									for(i=0;i<5;i++)
									{
											RxBuffer2[i]=0x00;      //�����������������
									}
						}
				} 
	
				else   //�����쳣
				{
						RxState2 = 0;
						RxCounter2=0;
						for(i=0;i<5;i++)
						{
								RxBuffer2[i]=0x00;      //�����������������
						}
				}
}
//�ж��Ƿ��а�ɫ�ϰ���
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
	printf("������Forbid\r\n");
}
