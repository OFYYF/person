#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "rc522.h" 
#include "beep.h"
#include "oled.h"
#include <stdbool.h>

	u8 KA[6]= {0xff,0xff,0xff,0xff,0xff,0xff};
	u8 data[16] = {0};
	u8 UID[4]= {0xA3, 0x2E, 0x3E, 0x4E};
	
	unsigned char sub_Money[4] = {0,0,0,0};      //充钱金额
  unsigned char Add_Money[4] = {0,0,0,0};      //充钱金额
	extern  char status;

	extern  char adr3_0;//	第3扇区0区块(第13块)
	unsigned int p;

		
//u8 RC522_lock(void);//刷卡解锁
//u8 cardid[6]={0,0,0,0,0,0};  //卡号1
////MFRC522数据区
//u8  mfrc552pidbuf[18];
//u8  card_pydebuf[2];
//u8  card_numberbuf[5];
//u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
//u8  card_writebuf[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//u8  card_readbuf[18];
 int main(void)
 {		
	delay_init();	    	 //延时函数初始化	  
	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
	KEY_Init();          //初始化与按键连接的硬件接口
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	RC522_Init();				//初始化射频卡模块
//	BEEP_Init();				//初始化蜂鸣器
	OLED_Init();		//初始化OLED
	OLED_Clear(); 		//清屏
	 	
	IC_RW (UID,0,KA,1,0x10,data );
	 	//IC_RW (UID,0,KEY_A,1,0x10,data );
	 
//	OLED_ShowCHinese(0,6,9); //刷
//	OLED_ShowCHinese(15,6,2); //卡
//	OLED_ShowCHinese(30,6,12); //显
//	OLED_ShowCHinese(45,6,13); //示
	 
	 
 	while(1)
	{
		  
			RC522_Handel();
			delay_ms(1000);
			delay_ms(1000);
			OLED_Clear(); 		//清屏
			OLED_ShowCHinese(0,6,9); //刷
			OLED_ShowCHinese(15,6,2); //卡
			OLED_ShowCHinese(30,6,12); //显
			OLED_ShowCHinese(45,6,13); //示

	}	 
//		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	
 } 
 
// void EXTI1_IRQHandler(void)//KEY0  
//{
//	delay_ms(10);
//	if(KEY0==1)	 	 
//	{				 
//		GPIO_ResetBits(GPIOC,GPIO_Pin_13);

//	}
//	EXTI_ClearITPendingBit(EXTI_Line1); 
//}
//void EXTI2_IRQHandler(void)//KEY1  
//{
//	
//	delay_ms(10);
//	if(KEY1==1)	
//	{				 
//		GPIO_SetBits(GPIOC,GPIO_Pin_13);

//	}
//	EXTI_ClearITPendingBit(EXTI_Line2); 
//} 

 void EXTI1_IRQHandler(void)//KEY0  
{
	  unsigned char date3_0[16];		//扇区3块0存放数据
	int p,i;
	delay_ms(10);
	if(KEY1==1)	 	 
	{		
			p=0;
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
							{
						Add_Money[0] = 0x01;    //为充值金额    //金额转化
						status = PcdValue(PICC_INCREMENT, adr3_0, Add_Money);
						printf("按键1执行A1\r\n");
						if (status == MI_OK)//充值成功
						{
						
								status=MI_ERR;
//							printf("执行到这啦");
								status=PcdRead(adr3_0,date3_0);
							
								if(status==MI_OK)//读卡成功
								{
										printf("date3_0:");
										for(i = 0; i < 16; i++)
											{
													printf("%02x", date3_0[i]);
											}
										printf("\r\n");
										printf("充值成功");
											
										Showoperate(p);//oled打印
										ShowMoney();//oled打印余额
											
										printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);
								}
								else 
										printf ("PcdRead failed\r\n");
						}
						else    
								printf ("PcdValue failed\r\n");
						}

	}
						delay_ms(1000);
						EXTI_ClearITPendingBit(EXTI_Line1); 
}
void EXTI0_IRQHandler(void)//KEY0  
{

  unsigned char date3_0[16];
	int i;
	delay_ms(10);
	if(KEY0==1)	 	 
				{	
					GPIO_SetBits(GPIOC,GPIO_Pin_13);
					p=1;
					printf("按键0执行A0\r\n");
				sub_Money[0] = 0x01;    //为扣款金额    //金额转化
					if((date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256)<(int)sub_Money[0])
					{
						printf("失败\r\n");
						
					}
					else
					{
						status = PcdValue(PICC_DECREMENT, adr3_0, sub_Money);
							if (status == MI_OK)//扣款成功
								{
						
									status=MI_ERR;
									status=PcdRead(adr3_0,date3_0);
           
									if(status==MI_OK)//读卡成功
										{
											printf("date3_0:");
												for(i = 0; i < 16; i++)
												{
														printf("%02x", date3_0[i]);
												}
														printf("\r\n");
														printf("charge success\r\n");
														printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);
														Showoperate(p);//oled打印
														ShowMoney();//oled打印余额
												
										}

								  else printf ("PcdRead failed\r\n");
								}
							 else    printf ("PcdValue failed\r\n");
					}
				}
					delay_ms(1000);
				EXTI_ClearITPendingBit(EXTI_Line0);
}

