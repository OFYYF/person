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
	
	unsigned char sub_Money[4] = {0,0,0,0};      //��Ǯ���
  unsigned char Add_Money[4] = {0,0,0,0};      //��Ǯ���
	extern  char status;

	extern  char adr3_0;//	��3����0����(��13��)
	unsigned int p;

		
//u8 RC522_lock(void);//ˢ������
//u8 cardid[6]={0,0,0,0,0,0};  //����1
////MFRC522������
//u8  mfrc552pidbuf[18];
//u8  card_pydebuf[2];
//u8  card_numberbuf[5];
//u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
//u8  card_writebuf[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//u8  card_readbuf[18];
 int main(void)
 {		
	delay_init();	    	 //��ʱ������ʼ��	  
	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	RC522_Init();				//��ʼ����Ƶ��ģ��
//	BEEP_Init();				//��ʼ��������
	OLED_Init();		//��ʼ��OLED
	OLED_Clear(); 		//����
	 	
	IC_RW (UID,0,KA,1,0x10,data );
	 	//IC_RW (UID,0,KEY_A,1,0x10,data );
	 
//	OLED_ShowCHinese(0,6,9); //ˢ
//	OLED_ShowCHinese(15,6,2); //��
//	OLED_ShowCHinese(30,6,12); //��
//	OLED_ShowCHinese(45,6,13); //ʾ
	 
	 
 	while(1)
	{
		  
			RC522_Handel();
			delay_ms(1000);
			delay_ms(1000);
			OLED_Clear(); 		//����
			OLED_ShowCHinese(0,6,9); //ˢ
			OLED_ShowCHinese(15,6,2); //��
			OLED_ShowCHinese(30,6,12); //��
			OLED_ShowCHinese(45,6,13); //ʾ

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
	  unsigned char date3_0[16];		//����3��0�������
	int p,i;
	delay_ms(10);
	if(KEY1==1)	 	 
	{		
			p=0;
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
							{
						Add_Money[0] = 0x01;    //Ϊ��ֵ���    //���ת��
						status = PcdValue(PICC_INCREMENT, adr3_0, Add_Money);
						printf("����1ִ��A1\r\n");
						if (status == MI_OK)//��ֵ�ɹ�
						{
						
								status=MI_ERR;
//							printf("ִ�е�����");
								status=PcdRead(adr3_0,date3_0);
							
								if(status==MI_OK)//�����ɹ�
								{
										printf("date3_0:");
										for(i = 0; i < 16; i++)
											{
													printf("%02x", date3_0[i]);
											}
										printf("\r\n");
										printf("��ֵ�ɹ�");
											
										Showoperate(p);//oled��ӡ
										ShowMoney();//oled��ӡ���
											
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
					printf("����0ִ��A0\r\n");
				sub_Money[0] = 0x01;    //Ϊ�ۿ���    //���ת��
					if((date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256)<(int)sub_Money[0])
					{
						printf("ʧ��\r\n");
						
					}
					else
					{
						status = PcdValue(PICC_DECREMENT, adr3_0, sub_Money);
							if (status == MI_OK)//�ۿ�ɹ�
								{
						
									status=MI_ERR;
									status=PcdRead(adr3_0,date3_0);
           
									if(status==MI_OK)//�����ɹ�
										{
											printf("date3_0:");
												for(i = 0; i < 16; i++)
												{
														printf("%02x", date3_0[i]);
												}
														printf("\r\n");
														printf("charge success\r\n");
														printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);
														Showoperate(p);//oled��ӡ
														ShowMoney();//oled��ӡ���
												
										}

								  else printf ("PcdRead failed\r\n");
								}
							 else    printf ("PcdValue failed\r\n");
					}
				}
					delay_ms(1000);
				EXTI_ClearITPendingBit(EXTI_Line0);
}

