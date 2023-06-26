#include "rc522.h"
#include "delay.h"
#include "usart.h"
#include <string.h>
#include "oled.h" 
#include "led.h"
#include "key.h"
#include <stdbool.h>
// M1����Ϊ16��������ÿ���������ĸ��飨��0����1����2����3�����
// ��16��������64���鰴���Ե�ַ���Ϊ��0~63
// ��0�������Ŀ�0�������Ե�ַ0�飩�����ڴ�ų��̴��룬�Ѿ��̻����ɸ��� 
// ÿ�������Ŀ�0����1����2Ϊ���ݿ飬�����ڴ������
// ÿ�������Ŀ�3Ϊ���ƿ飨���Ե�ַΪ:��3����7����11.....����������A����ȡ���ơ�����B��
 
/*******************************
*����˵����
*1--SDA  <----->PB12
*2--SCK  <----->PB13
*3--MOSI <----->PB15
*4--MISO <----->PB14
*5--����
*6--GND <----->GND
*7--RST <----->PA8
*8--VCC <----->VCC
************************************/
 
/*ȫ�ֱ���*/

//unsigned char RFID[16];			//���RFID 
//unsigned char lxl[4]={196,58,104,217};


unsigned char CT[2];//������
unsigned char SN[4]; //����
unsigned char date[16];			//�������
unsigned char date1_0[16];		//����1��0�������
unsigned char	date1_3[16];		//����1��3�������
unsigned char	date2_3[16];		//����2��3�������
unsigned char	date2_0[16];		//����2��0�������
unsigned char	date3_0[16];		//����3��0�������
unsigned char	date3_3[16];		//����3��3�������
unsigned char	date4_3[16];		//����4��3�������
unsigned char card0_bit=0;
unsigned char card1_bit=0;
unsigned char card2_bit=0;
unsigned char card3_bit=0;
unsigned char card4_bit=0;
unsigned char total=0;
// �滻���Լ�����UID

//A32E3E4E
unsigned char card_0[4]= {163,46,62,78};//���Լ��Ŀ�
//unsigned char card_2[4]= {121,209,130,123};//ˮ��
//unsigned char Add_Money[4] = {0,0,0,0};      //��Ǯ���
//unsigned char sub_Money[4] = {0,0,0,0};      //�ۿ���
u8 KEY_A[6]= {0xff,0xff,0xff,0xff,0xff,0xff};
u8 KEY_B[6]= {0xff,0xff,0xff,0xff,0xff,0xff};

// ������
unsigned char DATA0[16]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char DATA1[16]= {0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x07,0x81,0x69,0xff,0xff,0xff,0xff,0xff,0xff};//��β����������0Ϊ���ݿ�
unsigned char DATA2[16]= {0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x0c,0xf3,0x0c,0xf3};//����3����0����Ϊ��ֵ��ṹ��Ǯ������ʼΪ0
unsigned char status;
 
unsigned char adr2_0=0x08;//  ��2����0���飨��9�飩
unsigned char adr1_0=0x04;//	��1����0����(��5��)
unsigned char adr1_3=0x07;//	��1����3����(��8��)
unsigned char adr2_3=0x0B;//	��2����3����(��12��)
unsigned char adr3_0=0x0C;//	��3����0����(��13��)
unsigned char adr3_3=0x0F;//	��3����3����(��16��)
unsigned char adr4_3=0x13;//	��4����3����(��20��)
unsigned char adr5_3=0x17;//	��5����3����(��24��)

//u8 AUDIO_OPEN[6] = {0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC};
/*��������*/
//unsigned char status;
//unsigned char s=0x08;
//unsigned char ShowON; 
 
#define   RC522_DELAY()  delay_us( 20 )

//ID
char ss[255];
//oled ���
char mn[255];
//money
char money[255];

void RC522_Handel(void)
{
    u8 i = 0;
    status = PcdRequest(PICC_REQALL,CT);//Ѱ��
 
    // printf("\r\nstatus>>>>>>%d\r\n", status);
 
    if(status==MI_OK)// Ѱ���ɹ�
    {
        status=MI_ERR;
        status = PcdAnticoll(SN);// ����ײ ���UID ����SN
			  ShowID(SN);
			printf("Ѱ���ɹ�\r\n");
    }
		else
			printf("Please swipe the card\r\n");
 
    if (status==MI_OK)// ����ײ�ɹ�
    {
			
        status = MI_ERR;
        //ShowID(SN); // ���ڴ�ӡ����ID�� UID
			
			printf("��ӡid�ɹ�\r\n");
 
			//printf("��һ��%c �ڶ���%c ������%c ���ĸ�%c",SN[0],SN[1],SN[2],SN[3]);
			// �ж��Ƿ�Ϊ�Լ��Ŀ������Լ��Ŀ���ִ�к���Ĳ�����
			if((SN[0]==card_0[0])&&(SN[1]==card_0[1])&&(SN[2]==card_0[2])&&(SN[3]==card_0[3]))
       {
            card0_bit=1;
            printf("\r\nwelcome \r\n");
					  status = PcdSelect(SN);
					
					if(status == MI_OK)//ѡ���ɹ�
					{
							status = MI_ERR;
							// ��֤A��Կ ���ַ ���� SN
							// ע�⣺�˿��ַֻ��Ҫָ��ĳһ�����Ϳ����ˣ���ֻ�ܶ���֤�����������ж�д����
							status = PcdAuthState(KEYA, adr3_3, KEY_A, SN);
							if(status == MI_OK)//��֤�ɹ�
							{
										printf("PcdAuthState(A) success\r\n");
							}
							else
							{
									printf("PcdAuthState(A) failed\r\n");
							}
							// ��֤B��Կ ���ַ ���� SN  
							status = PcdAuthState(KEYB, adr3_3, KEY_B, SN);
							if(status == MI_OK)//��֤�ɹ�
							{
									printf("PcdAuthState(B) success\r\n");
							}
							else
							{
									printf("PcdAuthState(B) failed\r\n");
							}
					}
					////////////////////////////////
//						if(status == MI_OK)//�����ɹ�
//					{
//							status = MI_ERR;
//							printf("Write the card after 1 second. Do not move the card!!!\r\n");
//							delay_ms(1000);
//							 //status = PcdWrite(addr, DATA2);
//							// д���ݵ�M1��һ��
//						printf("��ʼд������\r\n");
//							status = PcdWrite(adr3_0, DATA2);
//							if(status == MI_OK)//д���ɹ�
//							{
//									printf("PcdWrite() success\r\n");
//							}
//							else
//							{
//									printf("PcdWrite() failed\r\n");
//									delay_ms(3000);
//							}
//					}
//					
//					if(status == MI_OK)//д���ɹ�
//					{
//							status = MI_ERR;
//							// ��ȡM1��һ������ ���ַ ��ȡ������
//							status = PcdRead(adr3_0, date3_0);
//							if(status == MI_OK)//�����ɹ�
//							{
//									// printf("DATA:%s\r\n", DATA);
//									printf("date3_0:");
//									for(i = 0; i < 16; i++)
//									{
//											printf("%02x", date3_0[i]);
//									}
//									printf("\r\n");
//							}
//							else
//							{
//									printf("PcdRead() failed\r\n");
//							}
//					}
					
					////////////////////////////////
					
					if(status == MI_OK)//��֤�ɹ�
					{
							status = MI_ERR;
							// ��ȡM1��һ������ ���ַ ��ȡ������ ע�⣺��Ϊ������֤��������3����������ֻ�ܶ�3���������ݽ��ж�д��������Χ��ȡʧ�ܡ�
							status = PcdRead(adr3_0, date3_0);
							if(status == MI_OK)//�����ɹ�
							{
									// printf("RFID:%s\r\n", RFID);
									printf("date3_0:");
									for(i = 0; i < 16; i++)
									{
										printf("%02x", date3_0[i]);
									}
								  printf("\r\n");
									printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);//��ӡ����������ж���Ǯ
									ShowMoney(); //��ʾ���
									printf("Press the KEY0 to recharge\r\n");//����KEY0��ֵ
									printf("Press the KEY1 to charge\r\n");//����KEY1�ۿ�

									
							}
							else
							{
									printf("PcdRead() failed\r\n");
							}
					}
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
//					if(KEY0==1)
//					{

//						Add_Money[0] = 0x01;    //Ϊ��ֵ���    //���ת��
//						status = PcdValue(PICC_INCREMENT, adr3_0, Add_Money);
//						printf("����0ִ��A1\r\n");
//						if (status == MI_OK)//��ֵ�ɹ�
//						{
//						
//								status=MI_ERR;
//							printf("ִ�е�����");
//								status=PcdRead(adr3_0,date3_0);
//							
//								printf("��ֵ�ɹ�");
//						
//								if(status==MI_OK)//�����ɹ�
//								{
//										printf("date3_0:");
//										for(i = 0; i < 16; i++)
//											{
//													printf("%02x", date3_0[i]);
//											}
//										printf("\r\n");
//										printf("recharge success\r\n");
//										printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);
//								}
//								else 
//										printf ("PcdRead failed\r\n");
//						}
//						else    
//								printf ("PcdValue failed\r\n");
//						}
//						 delay_ms(10);
			
//				if(KEY1==1)
//				{
//					printf("����1ִ��A2\r\n");
//				sub_Money[0] = 0x01;    //Ϊ�ۿ���    //���ת��
//					if((date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256)<(int)sub_Money[0])
//					{
//						printf("ʧ��\r\n");
//						
//					}
//					else
//					{
//						status = PcdValue(PICC_DECREMENT, adr3_0, sub_Money);
//							if (status == MI_OK)//�ۿ�ɹ�
//								{
//						
//									status=MI_ERR;
//									status=PcdRead(adr3_0,date3_0);
//           
//									if(status==MI_OK)//�����ɹ�
//										{
//											printf("date3_0:");
//												for(i = 0; i < 16; i++)
//												{
//														printf("%02x", date3_0[i]);
//												}
//														printf("\r\n");
//														printf("charge success\r\n");
//														printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);
//										}
//								  else printf ("PcdRead failed\r\n");
//								}
//							 else    printf ("PcdValue failed\r\n");
//					}
//				}
//				delay_ms(10);
//        status =PcdSelect(SN);
 
    }
    else
    {
        printf("\r\nThis card does not match\r\n");
       // TODO 
    }

					
//    if(status==MI_OK)//�x���ɹ�
//    {
// 
//        status=MI_ERR;
//        status =PcdAuthState(0x60,0x09,KEY,SN);
//     }
//    if(status==MI_OK)//��C�ɹ�
//    {
//        status=MI_ERR;
//        status=PcdRead(s,RFID);
//    }
// 
//    if(status==MI_OK)//�x���ɹ�
//    {
//        status=MI_ERR;
//        delay_ms(100);
//    }	
 
	}
				else
				{
					printf("ʶ�𿨺�ʧ��");
				}
}

void RC522_Init ( void )
{
	SPI1_Init();
	
	RC522_Reset_Disable();
	
	RC522_CS_Disable();
    
  PcdReset ();
    
	M500PcdConfigISOType ( 'A' );//���ù�����ʽ
 
}
 
void SPI1_Init(void)	
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
		
		// CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��PB12
    
    // SCK
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // MISO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // RST
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
}
 
 
/*
 * ��������SPI_RC522_SendByte
 * ����  ����RC522����1 Byte ����
 * ����  ��byte��Ҫ���͵�����
 * ����  : RC522���ص�����
 * ����  ���ڲ�����
 */
void SPI_RC522_SendByte ( u8 byte )
{
    u8 counter;
	
    for(counter=0;counter<8;counter++)
    {     
			if ( byte & 0x80 )
					RC522_MOSI_1 ();
			else 
					RC522_MOSI_0 ();
 
//			delay_us ( 3 );
			RC522_DELAY();
		
			RC522_SCK_0 ();
 
//			delay_us ( 1 );
//			delay_us ( 3 );
			RC522_DELAY();
			 
			RC522_SCK_1();
 
//			delay_us ( 3 );
			RC522_DELAY();
			 
			byte <<= 1; 
			
    } 
	
}
 
 
/*
 * ��������SPI_RC522_ReadByte
 * ����  ����RC522����1 Byte ����
 * ����  ����
 * ����  : RC522���ص�����
 * ����  ���ڲ�����
 */
u8 SPI_RC522_ReadByte ( void )
{
	u8 counter;
	u8 SPI_Data;
 
 
	for(counter=0;counter<8;counter++)
	{
			SPI_Data <<= 1;
	 
			RC522_SCK_0 ();
 
//			delay_us ( 3 );
		    RC522_DELAY();
		
			if ( RC522_MISO_GET() == 1)
					SPI_Data |= 0x01;
 
//			delay_us ( 2 );
//			delay_us ( 3 );
			RC522_DELAY();
 
			RC522_SCK_1 ();
	
//			delay_us ( 3 );
			RC522_DELAY();
			
	}
	
 
//	printf("****%c****",SPI_Data);
	return SPI_Data;
}
 
 
/*
 * ��������ReadRawRC
 * ����  ����RC522�Ĵ���
 * ����  ��ucAddress���Ĵ�����ַ
 * ����  : �Ĵ����ĵ�ǰֵ
 * ����  ���ڲ�����
 */
u8 ReadRawRC ( u8 ucAddress )
{
	u8 ucAddr, ucReturn;
	
	
	ucAddr = ( ( ucAddress << 1 ) & 0x7E ) | 0x80;
	
	RC522_CS_Enable();
	
	SPI_RC522_SendByte ( ucAddr );
	
	ucReturn = SPI_RC522_ReadByte ();
	
	RC522_CS_Disable();
 
	return ucReturn;
}
 
 
/*
 * ��������WriteRawRC
 * ����  ��дRC522�Ĵ���
 * ����  ��ucAddress���Ĵ�����ַ
 *         ucValue��д��Ĵ�����ֵ
 * ����  : ��
 * ����  ���ڲ�����
 */
void WriteRawRC ( u8 ucAddress, u8 ucValue )
{  
	u8 ucAddr;
 
	ucAddr = ( ucAddress << 1 ) & 0x7E;
	
	RC522_CS_Enable();
	
	SPI_RC522_SendByte ( ucAddr );
	
	SPI_RC522_SendByte ( ucValue );
	
	RC522_CS_Disable();	
}
 
 
/*
 * ��������SetBitMask
 * ����  ����RC522�Ĵ�����λ
 * ����  ��ucReg���Ĵ�����ַ
 *         ucMask����λֵ
 * ����  : ��
 * ����  ���ڲ�����
 */
void SetBitMask ( u8 ucReg, u8 ucMask )  
{
    u8 ucTemp;
 
    ucTemp = ReadRawRC ( ucReg );
	
    WriteRawRC ( ucReg, ucTemp | ucMask );         // set bit mask
 
}
 
 
/*
 * ��������ClearBitMask
 * ����  ����RC522�Ĵ�����λ
 * ����  ��ucReg���Ĵ�����ַ
 *         ucMask����λֵ
 * ����  : ��
 * ����  ���ڲ�����
 */
void ClearBitMask ( u8 ucReg, u8 ucMask )  
{
    u8 ucTemp;
 
    ucTemp = ReadRawRC ( ucReg );
	
    WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  // clear bit mask
	
	
}
 
 
/*
 * ��������PcdAntennaOn
 * ����  ���������� 
 * ����  ����
 * ����  : ��
 * ����  ���ڲ�����
 */
void PcdAntennaOn ( void )
{
    u8 uc;
 
    uc = ReadRawRC ( TxControlReg );
	
    if ( ! ( uc & 0x03 ) )
			SetBitMask(TxControlReg, 0x03);
	
}
 
 
/*
 * ��������PcdAntennaOff
 * ����  ���������� 
 * ����  ����
 * ����  : ��
 * ����  ���ڲ�����
 */
void PcdAntennaOff ( void )
{
 
    ClearBitMask ( TxControlReg, 0x03 );
 
}
 
 
/*
 * ��������PcdRese
 * ����  ����λRC522 
 * ����  ����
 * ����  : ��
 * ����  ���ⲿ����
 */
void PcdReset ( void )
{
    RC522_Reset_Disable();
 
    delay_us ( 1 );
 
    RC522_Reset_Enable();
 
    delay_us ( 1 );
 
    RC522_Reset_Disable();
 
    delay_us ( 1 );
 
    WriteRawRC ( CommandReg, 0x0f );
 
    while ( ReadRawRC ( CommandReg ) & 0x10 );
 
    delay_us ( 1 );
 
    WriteRawRC ( ModeReg, 0x3D );            //���巢�ͺͽ��ճ���ģʽ ��Mifare��ͨѶ��CRC��ʼֵ0x6363
 
    WriteRawRC ( TReloadRegL, 30 );          //16λ��ʱ����λ    
    WriteRawRC ( TReloadRegH, 0 );			     //16λ��ʱ����λ
 
    WriteRawRC ( TModeReg, 0x8D );				   //�����ڲ���ʱ��������
 
    WriteRawRC ( TPrescalerReg, 0x3E );			 //���ö�ʱ����Ƶϵ��
 
    WriteRawRC ( TxAutoReg, 0x40 );				   //���Ʒ����ź�Ϊ100%ASK	
	
 
}
 
 
/*
 * ��������M500PcdConfigISOType
 * ����  ������RC522�Ĺ�����ʽ
 * ����  ��ucType��������ʽ
 * ����  : ��
 * ����  ���ⲿ����
 */
void M500PcdConfigISOType ( u8 ucType )
{
	if ( ucType == 'A')                     //ISO14443_A
  {
		ClearBitMask ( Status2Reg, 0x08 );
		
    WriteRawRC ( ModeReg, 0x3D );//3F
		
		WriteRawRC ( RxSelReg, 0x86 );//84
		
		WriteRawRC( RFCfgReg, 0x7F );   //4F
		
		WriteRawRC( TReloadRegL, 30 );//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		
		WriteRawRC ( TReloadRegH, 0 );
		
		WriteRawRC ( TModeReg, 0x8D );
		
		WriteRawRC ( TPrescalerReg, 0x3E );
		
		delay_us ( 2 );
		
		PcdAntennaOn ();//������
		
   }
 
}
 
 

/*
 * ��������PcdComMF522
 * ����  ��ͨ��RC522��ISO14443��ͨѶ
 * ����  ��ucCommand��RC522������
 *         pInData��ͨ��RC522���͵���Ƭ������
 *         ucInLenByte���������ݵ��ֽڳ���
 *         pOutData�����յ��Ŀ�Ƭ��������
 *         pOutLenBit���������ݵ�λ����
 * ����  : ״ֵ̬
 *         = MI_OK���ɹ�
 * ����  ���ڲ�����
 */
char PcdComMF522 ( u8 ucCommand, u8 * pInData, u8 ucInLenByte, u8 * pOutData, u32 * pOutLenBit )
{
    char cStatus = MI_ERR;
    u8 ucIrqEn   = 0x00;
    u8 ucWaitFor = 0x00;
    u8 ucLastBits;
    u8 ucN;
    u32 ul;
 
    switch ( ucCommand )
    {
    case PCD_AUTHENT:		//Mifare��֤
        ucIrqEn   = 0x12;		//��������ж�����ErrIEn  ��������ж�IdleIEn
        ucWaitFor = 0x10;		//��֤Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ
        break;
 
    case PCD_TRANSCEIVE:		//���շ��� ���ͽ���
        ucIrqEn   = 0x77;		//����TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30;		//Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ�� �����жϱ�־λ
        break;
 
    default:
        break;
    }
 
    WriteRawRC ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv��λ�ܽ�IRQ��Status1Reg��IRqλ��ֵ�෴
    ClearBitMask ( ComIrqReg, 0x80 );			//Set1��λ����ʱ��CommIRqReg������λ����
    WriteRawRC ( CommandReg, PCD_IDLE );		//д��������
    SetBitMask ( FIFOLevelReg, 0x80 );			//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����
 
    for ( ul = 0; ul < ucInLenByte; ul ++ )
        WriteRawRC ( FIFODataReg, pInData [ ul ] );    		//д���ݽ�FIFOdata
 
    WriteRawRC ( CommandReg, ucCommand );					//д����
 
    if ( ucCommand == PCD_TRANSCEIVE )
        SetBitMask(BitFramingReg,0x80);  				//StartSend��λ�������ݷ��� ��λ���շ�����ʹ��ʱ����Ч
 
    ul = 1000;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
 
    do 														//��֤ ��Ѱ���ȴ�ʱ��
    {
        ucN = ReadRawRC ( ComIrqReg );							//��ѯ�¼��ж�
        ul --;
    } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//�˳�����i=0,��ʱ���жϣ���д��������
 
    ClearBitMask ( BitFramingReg, 0x80 );					//��������StartSendλ
 
    if ( ul != 0 )
    {
        if ( ! (( ReadRawRC ( ErrorReg ) & 0x1B )) )			//�������־�Ĵ���BufferOfI CollErr ParityErr ProtocolErr
        {
            cStatus = MI_OK;
					//printf("�������־�Ĵ����ɹ�\r\n");
 
            if ( ucN & ucIrqEn & 0x01 ){					//�Ƿ�����ʱ���ж�
                cStatus = MI_NOTAGERR;
						
								//printf("������ʱ��\r\n");
						}
 
            if ( ucCommand == PCD_TRANSCEIVE )
            {
                ucN = ReadRawRC ( FIFOLevelReg );			//��FIFO�б�����ֽ���
 
                ucLastBits = ReadRawRC ( ControlReg ) & 0x07;	//�����յ����ֽڵ���Чλ��
 
                if ( ucLastBits )
                    * pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N���ֽ�����ȥ1�����һ���ֽڣ�+���һλ��λ�� ��ȡ����������λ��
                else
                    * pOutLenBit = ucN * 8;   					//�����յ����ֽ������ֽ���Ч
 
                if ( ucN == 0 )
                    ucN = 1;
 
                if ( ucN > MAXRLEN )
                    ucN = MAXRLEN;
 
                for ( ul = 0; ul < ucN; ul ++ )
                    pOutData [ ul ] = ReadRawRC ( FIFODataReg );
            }
        }
        else{
            cStatus = MI_ERR;
				//printf("δ֪ʧ��\r\n");
				}
//			printf(ErrorReg);
    }
		//printf("��������\r\n");
    SetBitMask ( ControlReg, 0x80 );           // stop timer now
    WriteRawRC ( CommandReg, PCD_IDLE );
		
//		if(cStatus ==MI_OK){
//			printf("�����ҿ���\r\n");
//		}
//		else
//			printf("���°�\r\n");
		
		
 
    return cStatus;
}


 
/*
 * ��������PcdRequest
 * ����  ��Ѱ��
 * ����  ��ucReq_code��Ѱ����ʽ
 *                     = 0x52��Ѱ��Ӧ�������з���14443A��׼�Ŀ�
 *                     = 0x26��Ѱδ��������״̬�Ŀ�
 *         pTagType����Ƭ���ʹ���
 *                   = 0x4400��Mifare_UltraLight
 *                   = 0x0400��Mifare_One(S50)
 *                   = 0x0200��Mifare_One(S70)
 *                   = 0x0800��Mifare_Pro(X))
 *                   = 0x4403��Mifare_DESFire
 * ����  : ״ֵ̬
 *         = MI_OK���ɹ�
 * ����  ���ⲿ����
 */
char PcdRequest ( u8 ucReq_code, u8 * pTagType )
{
    char cStatus;  
    u8 ucComMF522Buf [ MAXRLEN ]; 
    u32 ulLen;
 
    ClearBitMask ( Status2Reg, 0x08 );	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
    WriteRawRC ( BitFramingReg, 0x07 );	//	���͵����һ���ֽڵ� ��λ
    SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2�ܽŵ�����źŴ��ݾ����͵��Ƶ�13.56�������ز��ź�
 
    ucComMF522Buf [ 0 ] = ucReq_code;		//���� ��Ƭ������
 
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//Ѱ��  
 
    if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//Ѱ���ɹ����ؿ����� 
    {    
       * pTagType = ucComMF522Buf [ 0 ];
       * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
    }
     
    else
     cStatus = MI_ERR;
 
    return cStatus;
 
}
 
 
/*
 * ��������PcdAnticoll
 * ����  ������ײ
 * ����  ��pSnr����Ƭ���кţ�4�ֽ�
 * ����  : ״ֵ̬
 *         = MI_OK���ɹ�
 * ����  ���ⲿ����
 */
char PcdAnticoll ( u8 * pSnr )
{
    char cStatus;
    u8 uc, ucSnr_check = 0;
    u8 ucComMF522Buf [ MAXRLEN ]; 
	u32 ulLen;
 
    ClearBitMask ( Status2Reg, 0x08 );		//��MFCryptol Onλ ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ
    WriteRawRC ( BitFramingReg, 0x00);		//����Ĵ��� ֹͣ�շ�
    ClearBitMask ( CollReg, 0x80 );			//��ValuesAfterColl���н��յ�λ�ڳ�ͻ�����
   
    ucComMF522Buf [ 0 ] = 0x93;	//��Ƭ����ͻ����
    ucComMF522Buf [ 1 ] = 0x20;
   
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//�뿨Ƭͨ��
	
    if ( cStatus == MI_OK)		//ͨ�ųɹ�
    {
		for ( uc = 0; uc < 4; uc ++ )
        {
            * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//����UID
            ucSnr_check ^= ucComMF522Buf [ uc ];
        }
			
        if ( ucSnr_check != ucComMF522Buf [ uc ] )
        		cStatus = MI_ERR;    
				 
    }
    
    SetBitMask ( CollReg, 0x80 );
 
    return cStatus;
	
}
 
 
/*
 * ��������CalulateCRC
 * ����  ����RC522����CRC16
 * ����  ��pIndata������CRC16������
 *         ucLen������CRC16�������ֽڳ���
 *         pOutData����ż�������ŵ��׵�ַ
 * ����  : ��
 * ����  ���ڲ�����
 */
void CalulateCRC ( u8 * pIndata, u8 ucLen, u8 * pOutData )
{
    u8 uc, ucN;
 
    ClearBitMask(DivIrqReg,0x04);
	
    WriteRawRC(CommandReg,PCD_IDLE);
	
    SetBitMask(FIFOLevelReg,0x80);
	
    for ( uc = 0; uc < ucLen; uc ++)
	    WriteRawRC ( FIFODataReg, * ( pIndata + uc ) );   
 
    WriteRawRC ( CommandReg, PCD_CALCCRC );
	
    uc = 0xFF;
	
    do 
    {
        ucN = ReadRawRC ( DivIrqReg );
        uc --;
    } while ( ( uc != 0 ) && ! ( ucN & 0x04 ) );
		
    pOutData [ 0 ] = ReadRawRC ( CRCResultRegL );
    pOutData [ 1 ] = ReadRawRC ( CRCResultRegM );
	
}
 
 
/*
 * ��������PcdSelect
 * ����  ��ѡ����Ƭ
 * ����  ��pSnr����Ƭ���кţ�4�ֽ�
 * ����  : ״ֵ̬
 *         = MI_OK���ɹ�
 * ����  ���ⲿ����
 */
char PcdSelect ( u8 * pSnr )
{
    char ucN;
    u8 uc;
	  u8 ucComMF522Buf [ MAXRLEN ]; 
    u32  ulLen;
 
    ucComMF522Buf [ 0 ] = PICC_ANTICOLL1;
    ucComMF522Buf [ 1 ] = 0x70;
    ucComMF522Buf [ 6 ] = 0;
	
    for ( uc = 0; uc < 4; uc ++ )
    {
    	ucComMF522Buf [ uc + 2 ] = * ( pSnr + uc );
    	ucComMF522Buf [ 6 ] ^= * ( pSnr + uc );
    }
		
    CalulateCRC ( ucComMF522Buf, 7, & ucComMF522Buf [ 7 ] );
  
    ClearBitMask ( Status2Reg, 0x08 );
 
    ucN = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, & ulLen );
    
    if ( ( ucN == MI_OK ) && ( ulLen == 0x18 ) ){
      ucN = MI_OK;
			printf("ѡ���ɹ�\r\n");}
    else{
      ucN = MI_ERR;
      printf("ѡ��ʧ��\r\n");    
		}
    return ucN;
	
}
 
 
/*
 * ��������PcdAuthState
 * ����  ����֤��Ƭ����
 * ����  ��ucAuth_mode��������֤ģʽ
 *                     = 0x60����֤A��Կ
 *                     = 0x61����֤B��Կ
 *         u8 ucAddr�����ַ
 *         pKey������
 *         pSnr����Ƭ���кţ�4�ֽ�
 * ����  : ״ֵ̬
 *         = MI_OK���ɹ�
 * ����  ���ⲿ����
 */
char PcdAuthState ( u8 ucAuth_mode, u8 ucAddr, u8 * pKey, u8 * pSnr )
{
    char cStatus;
	  u8 uc, ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;
 
    ucComMF522Buf [ 0 ] = ucAuth_mode;
    ucComMF522Buf [ 1 ] = ucAddr;
	
    for ( uc = 0; uc < 6; uc ++ )
	    ucComMF522Buf [ uc + 2 ] = * ( pKey + uc );   
	
    for ( uc = 0; uc < 6; uc ++ )
	    ucComMF522Buf [ uc + 8 ] = * ( pSnr + uc );   
 
    cStatus = PcdComMF522 ( PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, & ulLen );
	
    if ( ( cStatus != MI_OK ) || ( ! ( ReadRawRC ( Status2Reg ) & 0x08 ) ) )
			
		{
//			if(cStatus != MI_OK)
//					printf("666")	;		
//			else
//				printf("888");
			cStatus = MI_ERR; 
    }
		
    return cStatus;
		
}
 
 
/*
 * ��������PcdWrite
 * ����  ��д���ݵ�M1��һ��
 * ����  ��u8 ucAddr�����ַ
 *         pData��д������ݣ�16�ֽ�
 * ����  : ״ֵ̬
 *         = MI_OK���ɹ�
 * ����  ���ⲿ����
 */
char PcdWrite ( u8 ucAddr, u8 * pData )
{
    char cStatus;
    u8 uc, ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;
 
    ucComMF522Buf [ 0 ] = PICC_WRITE;
    ucComMF522Buf [ 1 ] = ucAddr;
 
    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
 
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
 
    if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
        cStatus = MI_ERR;
 
    if ( cStatus == MI_OK )
    {
        memcpy(ucComMF522Buf, pData, 16);
        for ( uc = 0; uc < 16; uc ++ )
            ucComMF522Buf [ uc ] = * ( pData + uc );
 
        CalulateCRC ( ucComMF522Buf, 16, & ucComMF522Buf [ 16 ] );
 
        cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, & ulLen );
 
        if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
            cStatus = MI_ERR;
 
    }
    return cStatus;
}
 
 
/*
 * ��������PcdRead
 * ����  ����ȡM1��һ������
 * ����  ��u8 ucAddr�����ַ
 *         pData�����������ݣ�16�ֽ�
 * ����  : ״ֵ̬
 *         = MI_OK���ɹ�
 * ����  ���ⲿ����
 */
char PcdRead ( u8 ucAddr, u8 * pData )
{
    char cStatus;
	  u8 uc, ucComMF522Buf [ MAXRLEN ]; 
    u32 ulLen;
 
    ucComMF522Buf [ 0 ] = PICC_READ;
    ucComMF522Buf [ 1 ] = ucAddr;
	
    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
   
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
	
    if ( ( cStatus == MI_OK ) && ( ulLen == 0x90 ) )
    {
			for ( uc = 0; uc < 16; uc ++ )
        * ( pData + uc ) = ucComMF522Buf [ uc ];   
    }
		
    else
      cStatus = MI_ERR;   
	
    return cStatus;
 
}
 
 
/*
 * ��������PcdHalt
 * ����  �����Ƭ��������״̬
 * ����  ����
 * ����  : ״ֵ̬
 *         = MI_OK���ɹ�
 * ����  ���ⲿ����
 */
char PcdHalt( void )
{
    u8 ucComMF522Buf [ MAXRLEN ]; 
    u32  ulLen;
 
    ucComMF522Buf [ 0 ] = PICC_HALT;
    ucComMF522Buf [ 1 ] = 0;
 
    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
    	PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
 
    return MI_OK;
	
}
 // UIDΪ��Ҫ�޸ĵĿ���UID key_type��0ΪKEYA����0ΪKEYB KEYΪ��Կ RW:1�Ƕ���0��д data_addrΪ�޸ĵĵ�ַ dataΪ��������
void IC_RW ( u8 * UID, u8 key_type, u8 * KEY, u8 RW, u8 data_addr, u8 * data )
{
	char status;
	u8 i = 0;
    u8 ucArray_ID [ 4 ] = { 0 };//�Ⱥ���IC�������ͺ�UID(IC�����к�)
 
    status = PcdRequest ( 0x52, ucArray_ID );//Ѱ��
	if(status == MI_OK)
		ShowID(ucArray_ID);
	else
		return;
 
    status = PcdAnticoll ( ucArray_ID );//����ײ
	if(status != MI_OK)
		return;
 
    status = PcdSelect ( UID );//ѡ����
	if(status != MI_OK)
	{
		printf("UID don't match\r\n");
		return;
	}
		
	if(0 == key_type)
		status = PcdAuthState ( KEYA, data_addr, KEY, UID );//У��
	else
		status = PcdAuthState ( KEYB, data_addr, KEY, UID );//У��
 
	if(status != MI_OK)
	{
		printf("KEY don't match\r\n");
		return;
	}
	
    if ( RW )//��дѡ��1�Ƕ���0��д
    {
		status = PcdRead ( data_addr, data );
		if(status == MI_OK)
		{
			printf("data:");
			for(i = 0; i < 16; i++)
            {
                printf("%02x", data[i]);
            }
            printf("\r\n");
		}
		else
		{
			printf("PcdRead() failed\r\n");
			return;
		}
	}
    else
	{
        status = PcdWrite ( data_addr, data );
		if(status == MI_OK)
		{
			printf("PcdWrite() finished\r\n");
		}
		else
		{
			printf("PcdWrite() failed\r\n");
			return;
		}
	}
 
    status = PcdHalt ();
	if(status == MI_OK)
	{
		printf("PcdHalt() finished\r\n");
	}
	else
	{
		printf("PcdHalt() failed\r\n");
		return;
	}
}
 
void IC_CMT ( u8 * UID, u8 * KEY, u8 RW, u8 * Dat )
{
    u8 ucArray_ID [ 4 ] = { 0 };//�Ⱥ���IC�������ͺ�UID(IC�����к�)
 
    PcdRequest ( 0x52, ucArray_ID );//Ѱ��
 
    PcdAnticoll ( ucArray_ID );//����ײ
 
    PcdSelect ( UID );//ѡ����
 
    PcdAuthState ( 0x60, 0x10, KEY, UID );//У��
 
    if ( RW )//��дѡ��1�Ƕ���0��д
        PcdRead ( 0x10, Dat );
 
    else 
        PcdWrite ( 0x10, Dat );
     
    PcdHalt ();	 
	 
}


//oled��ʾ��ֵ��ۿ�
 void Showoperate(int p){
 					if(p==0)
					{
						OLED_ShowCHinese(0,3,14); //��ֵ�ɹ�
						OLED_ShowCHinese(15,3,15);
						OLED_ShowCHinese(30,3,16);
						OLED_ShowCHinese(45,3,17);
					}

					if(p==1)
					{
						OLED_ShowCHinese(0,3,18); //�ۿ�ɹ�
						OLED_ShowCHinese(15,3,19);
						OLED_ShowCHinese(30,3,16);
						OLED_ShowCHinese(45,3,17);
					}
 
 }


//oled��ʾ���
 void ShowMoney(void)
{
	 u8 num;
//	int num;
   status = PcdRead(adr3_0, date3_0);
//	 OLED_Clear();
	  if(status == MI_OK)//�����ɹ�
				{
									// printf("RFID:%s\r\n", RFID);
//					printf("date3_0:");
//					for(i = 0; i < 16; i++)
//					{
//						  printf("%02x", date3_0[i]);
//					}
						  //printf("\r\n");
						  //printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);//��ӡ����������ж���Ǯ
					    num=date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256;
					printf("oled��ʾ��\r\n");
					printf("%d",num);
					
//					sprintf(mn,"%s",num);
					
					sprintf(mn,"%d",num);
					OLED_ShowString(0,2,"money:",8); //oled��ʾ
					OLED_ShowString(50,2,(u8*)mn,8);

				
}	 

}


void ShowID(u8 *p)	 //��ʾ���Ŀ��ţ���ʮ��������ʾ
{
	u8 num[9];
	u8 i;
 
 	for(i=0;i<4;i++)
	{
		num[i*2]=p[i]/16;
		num[i*2]>9?(num[i*2]+='7'):(num[i*2]+='0');
		num[i*2+1]=p[i]%16;
		num[i*2+1]>9?(num[i*2+1]+='7'):(num[i*2+1]+='0');
		//printf("your card id is %s\r\n", num);
	}
	
		sprintf(ss,"%s",num);
		OLED_ShowString(0,0,"ID:",8); //oled��ʾ
		OLED_ShowString(25,0,(u8*)ss,8); //oled��ʾ 
	
	  num[8] = 0;
    printf("your card id is %s\r\n", num);
	
	  printf("ID>>>%s\r\n", num);
		
	
//	num[8]=0;
//	for(i =0;i<10;i++)
//		{
//		    SN[i] = num[i];
//		}


//		if(strcmp(SN,"A32E3E4E") == 0)
//		{
//		    LED0 = 0;
//			delay_ms(500);
//			LED0=1;
//		}
//		if(strcmp(SN,"9EDE0D1D") == 0)
//		{
//		    LED1 = 0;
//			delay_ms(500);
//			LED1=1;
//		}
//		
//		else 
//		{
////			OLED_ShowString(0,5,"error",16); //oled��ʾ
//		}
	


 
}
//�ȴ����뿪
void WaitCardOff(void)
{
    char status;
    unsigned char	TagType[2];
 
    while(1)
    {
			
        status = PcdRequest(REQ_ALL, TagType);
        if(status)
        {
            status = PcdRequest(REQ_ALL, TagType);
            if(status)
            {
                status = PcdRequest(REQ_ALL, TagType);
                if(status)
                {
                    return;
                }
            }
        }
        delay_ms(1000);
    }
}
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
 
//status = PcdValue(PICC_INCREMENT, adr3_0, Add_Money);

char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	
//			if(status ==MI_OK){
//			printf("123�����ҿ���\r\n");
//		}
//		else
//			printf("123���°�\r\n");
	
		// || ((ucComMF522Buf[0] & 0x0F) != 0x0A)
    if ((status != MI_OK) || (unLen != 4)|| ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
			status = MI_ERR;
				//printf("��һ��\r\n");
			 // printf("unLen:%d",unLen);
			  //printf("ucComMF522Buf:%c\r\n",ucComMF522Buf[0]);
		}
        
    if (status == MI_OK)
    {
			//printf("ִ������\r\n");
       // memcpy(ucComMF522Buf, pValue, 4);
        for (i=0; i<16; i++)
        {    
					ucComMF522Buf[i] = *(pValue+i);   
				}
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
				//printf("�ڶ���\r\n");
        if (status != MI_ERR)
        {  
					status = MI_OK;    
				}
				//printf("������\r\n");
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  //|| ((ucComMF522Buf[0] & 0x0F) != 0x0A)
				if ((status != MI_OK) || (unLen != 4)|| ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   
					
//								if(status ==MI_OK)
//								{
//			          printf("123456�����ҿ���\r\n");
//		            }
//		            else
//			          printf("123456���°�\r\n");
		
					status = MI_ERR;   
				}
				//printf("���ĸ�\r\n");
    }
		
		//printf("����ִ�����\r\n");
    return status;

}
