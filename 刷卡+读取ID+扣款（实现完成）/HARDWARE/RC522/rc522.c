#include "rc522.h"
#include "delay.h"
#include "usart.h"
#include <string.h>
#include "oled.h" 
#include "led.h"
#include "key.h"
#include <stdbool.h>
// M1卡分为16个扇区，每个扇区由四个块（块0、块1、块2、块3）组成
// 将16个扇区的64个块按绝对地址编号为：0~63
// 第0个扇区的块0（即绝对地址0块），用于存放厂商代码，已经固化不可更改 
// 每个扇区的块0、块1、块2为数据块，可用于存放数据
// 每个扇区的块3为控制块（绝对地址为:块3、块7、块11.....）包括密码A，存取控制、密码B等
 
/*******************************
*连线说明：
*1--SDA  <----->PB12
*2--SCK  <----->PB13
*3--MOSI <----->PB15
*4--MISO <----->PB14
*5--悬空
*6--GND <----->GND
*7--RST <----->PA8
*8--VCC <----->VCC
************************************/
 
/*全局变量*/

//unsigned char RFID[16];			//存放RFID 
//unsigned char lxl[4]={196,58,104,217};


unsigned char CT[2];//卡类型
unsigned char SN[4]; //卡号
unsigned char date[16];			//存放数据
unsigned char date1_0[16];		//扇区1块0存放数据
unsigned char	date1_3[16];		//扇区1块3存放数据
unsigned char	date2_3[16];		//扇区2块3存放数据
unsigned char	date2_0[16];		//扇区2块0存放数据
unsigned char	date3_0[16];		//扇区3块0存放数据
unsigned char	date3_3[16];		//扇区3块3存放数据
unsigned char	date4_3[16];		//扇区4块3存放数据
unsigned char card0_bit=0;
unsigned char card1_bit=0;
unsigned char card2_bit=0;
unsigned char card3_bit=0;
unsigned char card4_bit=0;
unsigned char total=0;
// 替换成自己卡的UID

//A32E3E4E
unsigned char card_0[4]= {163,46,62,78};//我自己的卡
//unsigned char card_2[4]= {121,209,130,123};//水卡
//unsigned char Add_Money[4] = {0,0,0,0};      //充钱金额
//unsigned char sub_Money[4] = {0,0,0,0};      //扣款金额
u8 KEY_A[6]= {0xff,0xff,0xff,0xff,0xff,0xff};
u8 KEY_B[6]= {0xff,0xff,0xff,0xff,0xff,0xff};

// 置零用
unsigned char DATA0[16]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char DATA1[16]= {0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x07,0x81,0x69,0xff,0xff,0xff,0xff,0xff,0xff};//改尾块设置扇区0为数据块
unsigned char DATA2[16]= {0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x0c,0xf3,0x0c,0xf3};//设置3扇区0块区为数值块结构，钱包余额初始为0
unsigned char status;
 
unsigned char adr2_0=0x08;//  第2扇区0区块（第9块）
unsigned char adr1_0=0x04;//	第1扇区0区块(第5块)
unsigned char adr1_3=0x07;//	第1扇区3区块(第8块)
unsigned char adr2_3=0x0B;//	第2扇区3区块(第12块)
unsigned char adr3_0=0x0C;//	第3扇区0区块(第13块)
unsigned char adr3_3=0x0F;//	第3扇区3区块(第16块)
unsigned char adr4_3=0x13;//	第4扇区3区块(第20块)
unsigned char adr5_3=0x17;//	第5扇区3区块(第24块)

//u8 AUDIO_OPEN[6] = {0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC};
/*函数声明*/
//unsigned char status;
//unsigned char s=0x08;
//unsigned char ShowON; 
 
#define   RC522_DELAY()  delay_us( 20 )

//ID
char ss[255];
//oled 存的
char mn[255];
//money
char money[255];

void RC522_Handel(void)
{
    u8 i = 0;
    status = PcdRequest(PICC_REQALL,CT);//寻卡
 
    // printf("\r\nstatus>>>>>>%d\r\n", status);
 
    if(status==MI_OK)// 寻卡成功
    {
        status=MI_ERR;
        status = PcdAnticoll(SN);// 防冲撞 获得UID 存入SN
			  ShowID(SN);
			printf("寻卡成功\r\n");
    }
		else
			printf("Please swipe the card\r\n");
 
    if (status==MI_OK)// 防冲撞成功
    {
			
        status = MI_ERR;
        //ShowID(SN); // 串口打印卡的ID号 UID
			
			printf("打印id成功\r\n");
 
			//printf("第一个%c 第二个%c 第三个%c 第四个%c",SN[0],SN[1],SN[2],SN[3]);
			// 判断是否为自己的卡，是自己的卡才执行后面的操作。
			if((SN[0]==card_0[0])&&(SN[1]==card_0[1])&&(SN[2]==card_0[2])&&(SN[3]==card_0[3]))
       {
            card0_bit=1;
            printf("\r\nwelcome \r\n");
					  status = PcdSelect(SN);
					
					if(status == MI_OK)//选卡成功
					{
							status = MI_ERR;
							// 验证A密钥 块地址 密码 SN
							// 注意：此块地址只需要指向某一扇区就可以了，且只能对验证过的扇区进行读写操作
							status = PcdAuthState(KEYA, adr3_3, KEY_A, SN);
							if(status == MI_OK)//验证成功
							{
										printf("PcdAuthState(A) success\r\n");
							}
							else
							{
									printf("PcdAuthState(A) failed\r\n");
							}
							// 验证B密钥 块地址 密码 SN  
							status = PcdAuthState(KEYB, adr3_3, KEY_B, SN);
							if(status == MI_OK)//验证成功
							{
									printf("PcdAuthState(B) success\r\n");
							}
							else
							{
									printf("PcdAuthState(B) failed\r\n");
							}
					}
					////////////////////////////////
//						if(status == MI_OK)//读卡成功
//					{
//							status = MI_ERR;
//							printf("Write the card after 1 second. Do not move the card!!!\r\n");
//							delay_ms(1000);
//							 //status = PcdWrite(addr, DATA2);
//							// 写数据到M1卡一块
//						printf("开始写入数据\r\n");
//							status = PcdWrite(adr3_0, DATA2);
//							if(status == MI_OK)//写卡成功
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
//					if(status == MI_OK)//写卡成功
//					{
//							status = MI_ERR;
//							// 读取M1卡一块数据 块地址 读取的数据
//							status = PcdRead(adr3_0, date3_0);
//							if(status == MI_OK)//读卡成功
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
					
					if(status == MI_OK)//验证成功
					{
							status = MI_ERR;
							// 读取M1卡一块数据 块地址 读取的数据 注意：因为上面验证的扇区是3扇区，所以只能对3扇区的数据进行读写，超出范围读取失败。
							status = PcdRead(adr3_0, date3_0);
							if(status == MI_OK)//读卡成功
							{
									// printf("RFID:%s\r\n", RFID);
									printf("date3_0:");
									for(i = 0; i < 16; i++)
									{
										printf("%02x", date3_0[i]);
									}
								  printf("\r\n");
									printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);//打印输出你现在有多少钱
									ShowMoney(); //显示余额
									printf("Press the KEY0 to recharge\r\n");//按下KEY0充值
									printf("Press the KEY1 to charge\r\n");//按下KEY1扣款

									
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

//						Add_Money[0] = 0x01;    //为充值金额    //金额转化
//						status = PcdValue(PICC_INCREMENT, adr3_0, Add_Money);
//						printf("按键0执行A1\r\n");
//						if (status == MI_OK)//充值成功
//						{
//						
//								status=MI_ERR;
//							printf("执行到这啦");
//								status=PcdRead(adr3_0,date3_0);
//							
//								printf("充值成功");
//						
//								if(status==MI_OK)//读卡成功
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
//					printf("按键1执行A2\r\n");
//				sub_Money[0] = 0x01;    //为扣款金额    //金额转化
//					if((date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256)<(int)sub_Money[0])
//					{
//						printf("失败\r\n");
//						
//					}
//					else
//					{
//						status = PcdValue(PICC_DECREMENT, adr3_0, sub_Money);
//							if (status == MI_OK)//扣款成功
//								{
//						
//									status=MI_ERR;
//									status=PcdRead(adr3_0,date3_0);
//           
//									if(status==MI_OK)//读卡成功
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

					
//    if(status==MI_OK)//選卡成功
//    {
// 
//        status=MI_ERR;
//        status =PcdAuthState(0x60,0x09,KEY,SN);
//     }
//    if(status==MI_OK)//驗證成功
//    {
//        status=MI_ERR;
//        status=PcdRead(s,RFID);
//    }
// 
//    if(status==MI_OK)//讀卡成功
//    {
//        status=MI_ERR;
//        delay_ms(100);
//    }	
 
	}
				else
				{
					printf("识别卡号失败");
				}
}

void RC522_Init ( void )
{
	SPI1_Init();
	
	RC522_Reset_Disable();
	
	RC522_CS_Disable();
    
  PcdReset ();
    
	M500PcdConfigISOType ( 'A' );//设置工作方式
 
}
 
void SPI1_Init(void)	
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
		
		// CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PB12
    
    // SCK
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // MISO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // RST
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
}
 
 
/*
 * 函数名：SPI_RC522_SendByte
 * 描述  ：向RC522发送1 Byte 数据
 * 输入  ：byte，要发送的数据
 * 返回  : RC522返回的数据
 * 调用  ：内部调用
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
 * 函数名：SPI_RC522_ReadByte
 * 描述  ：从RC522发送1 Byte 数据
 * 输入  ：无
 * 返回  : RC522返回的数据
 * 调用  ：内部调用
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
 * 函数名：ReadRawRC
 * 描述  ：读RC522寄存器
 * 输入  ：ucAddress，寄存器地址
 * 返回  : 寄存器的当前值
 * 调用  ：内部调用
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
 * 函数名：WriteRawRC
 * 描述  ：写RC522寄存器
 * 输入  ：ucAddress，寄存器地址
 *         ucValue，写入寄存器的值
 * 返回  : 无
 * 调用  ：内部调用
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
 * 函数名：SetBitMask
 * 描述  ：对RC522寄存器置位
 * 输入  ：ucReg，寄存器地址
 *         ucMask，置位值
 * 返回  : 无
 * 调用  ：内部调用
 */
void SetBitMask ( u8 ucReg, u8 ucMask )  
{
    u8 ucTemp;
 
    ucTemp = ReadRawRC ( ucReg );
	
    WriteRawRC ( ucReg, ucTemp | ucMask );         // set bit mask
 
}
 
 
/*
 * 函数名：ClearBitMask
 * 描述  ：对RC522寄存器清位
 * 输入  ：ucReg，寄存器地址
 *         ucMask，清位值
 * 返回  : 无
 * 调用  ：内部调用
 */
void ClearBitMask ( u8 ucReg, u8 ucMask )  
{
    u8 ucTemp;
 
    ucTemp = ReadRawRC ( ucReg );
	
    WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  // clear bit mask
	
	
}
 
 
/*
 * 函数名：PcdAntennaOn
 * 描述  ：开启天线 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：内部调用
 */
void PcdAntennaOn ( void )
{
    u8 uc;
 
    uc = ReadRawRC ( TxControlReg );
	
    if ( ! ( uc & 0x03 ) )
			SetBitMask(TxControlReg, 0x03);
	
}
 
 
/*
 * 函数名：PcdAntennaOff
 * 描述  ：开启天线 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：内部调用
 */
void PcdAntennaOff ( void )
{
 
    ClearBitMask ( TxControlReg, 0x03 );
 
}
 
 
/*
 * 函数名：PcdRese
 * 描述  ：复位RC522 
 * 输入  ：无
 * 返回  : 无
 * 调用  ：外部调用
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
 
    WriteRawRC ( ModeReg, 0x3D );            //定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
 
    WriteRawRC ( TReloadRegL, 30 );          //16位定时器低位    
    WriteRawRC ( TReloadRegH, 0 );			     //16位定时器高位
 
    WriteRawRC ( TModeReg, 0x8D );				   //定义内部定时器的设置
 
    WriteRawRC ( TPrescalerReg, 0x3E );			 //设置定时器分频系数
 
    WriteRawRC ( TxAutoReg, 0x40 );				   //调制发送信号为100%ASK	
	
 
}
 
 
/*
 * 函数名：M500PcdConfigISOType
 * 描述  ：设置RC522的工作方式
 * 输入  ：ucType，工作方式
 * 返回  : 无
 * 调用  ：外部调用
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
		
		PcdAntennaOn ();//开天线
		
   }
 
}
 
 

/*
 * 函数名：PcdComMF522
 * 描述  ：通过RC522和ISO14443卡通讯
 * 输入  ：ucCommand，RC522命令字
 *         pInData，通过RC522发送到卡片的数据
 *         ucInLenByte，发送数据的字节长度
 *         pOutData，接收到的卡片返回数据
 *         pOutLenBit，返回数据的位长度
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：内部调用
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
    case PCD_AUTHENT:		//Mifare认证
        ucIrqEn   = 0x12;		//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
        ucWaitFor = 0x10;		//认证寻卡等待时候 查询空闲中断标志位
        break;
 
    case PCD_TRANSCEIVE:		//接收发送 发送接收
        ucIrqEn   = 0x77;		//允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30;		//寻卡等待时候 查询接收中断标志位与 空闲中断标志位
        break;
 
    default:
        break;
    }
 
    WriteRawRC ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反
    ClearBitMask ( ComIrqReg, 0x80 );			//Set1该位清零时，CommIRqReg的屏蔽位清零
    WriteRawRC ( CommandReg, PCD_IDLE );		//写空闲命令
    SetBitMask ( FIFOLevelReg, 0x80 );			//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
 
    for ( ul = 0; ul < ucInLenByte; ul ++ )
        WriteRawRC ( FIFODataReg, pInData [ ul ] );    		//写数据进FIFOdata
 
    WriteRawRC ( CommandReg, ucCommand );					//写命令
 
    if ( ucCommand == PCD_TRANSCEIVE )
        SetBitMask(BitFramingReg,0x80);  				//StartSend置位启动数据发送 该位与收发命令使用时才有效
 
    ul = 1000;//根据时钟频率调整，操作M1卡最大等待时间25ms
 
    do 														//认证 与寻卡等待时间
    {
        ucN = ReadRawRC ( ComIrqReg );							//查询事件中断
        ul --;
    } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//退出条件i=0,定时器中断，与写空闲命令
 
    ClearBitMask ( BitFramingReg, 0x80 );					//清理允许StartSend位
 
    if ( ul != 0 )
    {
        if ( ! (( ReadRawRC ( ErrorReg ) & 0x1B )) )			//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
        {
            cStatus = MI_OK;
					//printf("读错误标志寄存器成功\r\n");
 
            if ( ucN & ucIrqEn & 0x01 ){					//是否发生定时器中断
                cStatus = MI_NOTAGERR;
						
								//printf("开启定时器\r\n");
						}
 
            if ( ucCommand == PCD_TRANSCEIVE )
            {
                ucN = ReadRawRC ( FIFOLevelReg );			//读FIFO中保存的字节数
 
                ucLastBits = ReadRawRC ( ControlReg ) & 0x07;	//最后接收到得字节的有效位数
 
                if ( ucLastBits )
                    * pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
                else
                    * pOutLenBit = ucN * 8;   					//最后接收到的字节整个字节有效
 
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
				//printf("未知失败\r\n");
				}
//			printf(ErrorReg);
    }
		//printf("到这了吗\r\n");
    SetBitMask ( ControlReg, 0x80 );           // stop timer now
    WriteRawRC ( CommandReg, PCD_IDLE );
		
//		if(cStatus ==MI_OK){
//			printf("告诉我可以\r\n");
//		}
//		else
//			printf("搞事啊\r\n");
		
		
 
    return cStatus;
}


 
/*
 * 函数名：PcdRequest
 * 描述  ：寻卡
 * 输入  ：ucReq_code，寻卡方式
 *                     = 0x52，寻感应区内所有符合14443A标准的卡
 *                     = 0x26，寻未进入休眠状态的卡
 *         pTagType，卡片类型代码
 *                   = 0x4400，Mifare_UltraLight
 *                   = 0x0400，Mifare_One(S50)
 *                   = 0x0200，Mifare_One(S70)
 *                   = 0x0800，Mifare_Pro(X))
 *                   = 0x4403，Mifare_DESFire
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdRequest ( u8 ucReq_code, u8 * pTagType )
{
    char cStatus;  
    u8 ucComMF522Buf [ MAXRLEN ]; 
    u32 ulLen;
 
    ClearBitMask ( Status2Reg, 0x08 );	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
    WriteRawRC ( BitFramingReg, 0x07 );	//	发送的最后一个字节的 七位
    SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号
 
    ucComMF522Buf [ 0 ] = ucReq_code;		//存入 卡片命令字
 
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//寻卡  
 
    if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//寻卡成功返回卡类型 
    {    
       * pTagType = ucComMF522Buf [ 0 ];
       * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
    }
     
    else
     cStatus = MI_ERR;
 
    return cStatus;
 
}
 
 
/*
 * 函数名：PcdAnticoll
 * 描述  ：防冲撞
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdAnticoll ( u8 * pSnr )
{
    char cStatus;
    u8 uc, ucSnr_check = 0;
    u8 ucComMF522Buf [ MAXRLEN ]; 
	u32 ulLen;
 
    ClearBitMask ( Status2Reg, 0x08 );		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    WriteRawRC ( BitFramingReg, 0x00);		//清理寄存器 停止收发
    ClearBitMask ( CollReg, 0x80 );			//清ValuesAfterColl所有接收的位在冲突后被清除
   
    ucComMF522Buf [ 0 ] = 0x93;	//卡片防冲突命令
    ucComMF522Buf [ 1 ] = 0x20;
   
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//与卡片通信
	
    if ( cStatus == MI_OK)		//通信成功
    {
		for ( uc = 0; uc < 4; uc ++ )
        {
            * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//读出UID
            ucSnr_check ^= ucComMF522Buf [ uc ];
        }
			
        if ( ucSnr_check != ucComMF522Buf [ uc ] )
        		cStatus = MI_ERR;    
				 
    }
    
    SetBitMask ( CollReg, 0x80 );
 
    return cStatus;
	
}
 
 
/*
 * 函数名：CalulateCRC
 * 描述  ：用RC522计算CRC16
 * 输入  ：pIndata，计算CRC16的数组
 *         ucLen，计算CRC16的数组字节长度
 *         pOutData，存放计算结果存放的首地址
 * 返回  : 无
 * 调用  ：内部调用
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
 * 函数名：PcdSelect
 * 描述  ：选定卡片
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
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
			printf("选卡成功\r\n");}
    else{
      ucN = MI_ERR;
      printf("选卡失败\r\n");    
		}
    return ucN;
	
}
 
 
/*
 * 函数名：PcdAuthState
 * 描述  ：验证卡片密码
 * 输入  ：ucAuth_mode，密码验证模式
 *                     = 0x60，验证A密钥
 *                     = 0x61，验证B密钥
 *         u8 ucAddr，块地址
 *         pKey，密码
 *         pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
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
 * 函数名：PcdWrite
 * 描述  ：写数据到M1卡一块
 * 输入  ：u8 ucAddr，块地址
 *         pData，写入的数据，16字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
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
 * 函数名：PcdRead
 * 描述  ：读取M1卡一块数据
 * 输入  ：u8 ucAddr，块地址
 *         pData，读出的数据，16字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
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
 * 函数名：PcdHalt
 * 描述  ：命令卡片进入休眠状态
 * 输入  ：无
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
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
 // UID为你要修改的卡的UID key_type：0为KEYA，非0为KEYB KEY为密钥 RW:1是读，0是写 data_addr为修改的地址 data为数据内容
void IC_RW ( u8 * UID, u8 key_type, u8 * KEY, u8 RW, u8 data_addr, u8 * data )
{
	char status;
	u8 i = 0;
    u8 ucArray_ID [ 4 ] = { 0 };//先后存放IC卡的类型和UID(IC卡序列号)
 
    status = PcdRequest ( 0x52, ucArray_ID );//寻卡
	if(status == MI_OK)
		ShowID(ucArray_ID);
	else
		return;
 
    status = PcdAnticoll ( ucArray_ID );//防冲撞
	if(status != MI_OK)
		return;
 
    status = PcdSelect ( UID );//选定卡
	if(status != MI_OK)
	{
		printf("UID don't match\r\n");
		return;
	}
		
	if(0 == key_type)
		status = PcdAuthState ( KEYA, data_addr, KEY, UID );//校验
	else
		status = PcdAuthState ( KEYB, data_addr, KEY, UID );//校验
 
	if(status != MI_OK)
	{
		printf("KEY don't match\r\n");
		return;
	}
	
    if ( RW )//读写选择，1是读，0是写
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
    u8 ucArray_ID [ 4 ] = { 0 };//先后存放IC卡的类型和UID(IC卡序列号)
 
    PcdRequest ( 0x52, ucArray_ID );//寻卡
 
    PcdAnticoll ( ucArray_ID );//防冲撞
 
    PcdSelect ( UID );//选定卡
 
    PcdAuthState ( 0x60, 0x10, KEY, UID );//校验
 
    if ( RW )//读写选择，1是读，0是写
        PcdRead ( 0x10, Dat );
 
    else 
        PcdWrite ( 0x10, Dat );
     
    PcdHalt ();	 
	 
}


//oled显示充值与扣款
 void Showoperate(int p){
 					if(p==0)
					{
						OLED_ShowCHinese(0,3,14); //充值成功
						OLED_ShowCHinese(15,3,15);
						OLED_ShowCHinese(30,3,16);
						OLED_ShowCHinese(45,3,17);
					}

					if(p==1)
					{
						OLED_ShowCHinese(0,3,18); //扣款成功
						OLED_ShowCHinese(15,3,19);
						OLED_ShowCHinese(30,3,16);
						OLED_ShowCHinese(45,3,17);
					}
 
 }


//oled显示余额
 void ShowMoney(void)
{
	 u8 num;
//	int num;
   status = PcdRead(adr3_0, date3_0);
//	 OLED_Clear();
	  if(status == MI_OK)//读卡成功
				{
									// printf("RFID:%s\r\n", RFID);
//					printf("date3_0:");
//					for(i = 0; i < 16; i++)
//					{
//						  printf("%02x", date3_0[i]);
//					}
						  //printf("\r\n");
						  //printf("now you have %d money\r\n",date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256);//打印输出你现在有多少钱
					    num=date3_0[0]+date3_0[1]*256+date3_0[2]*256*256+date3_0[3]*256*256*256;
					printf("oled显示：\r\n");
					printf("%d",num);
					
//					sprintf(mn,"%s",num);
					
					sprintf(mn,"%d",num);
					OLED_ShowString(0,2,"money:",8); //oled显示
					OLED_ShowString(50,2,(u8*)mn,8);

				
}	 

}


void ShowID(u8 *p)	 //显示卡的卡号，以十六进制显示
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
		OLED_ShowString(0,0,"ID:",8); //oled显示
		OLED_ShowString(25,0,(u8*)ss,8); //oled显示 
	
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
////			OLED_ShowString(0,5,"error",16); //oled显示
//		}
	


 
}
//等待卡离开
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
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
 
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
//			printf("123告诉我可以\r\n");
//		}
//		else
//			printf("123搞事啊\r\n");
	
		// || ((ucComMF522Buf[0] & 0x0F) != 0x0A)
    if ((status != MI_OK) || (unLen != 4)|| ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
			status = MI_ERR;
				//printf("第一个\r\n");
			 // printf("unLen:%d",unLen);
			  //printf("ucComMF522Buf:%c\r\n",ucComMF522Buf[0]);
		}
        
    if (status == MI_OK)
    {
			//printf("执行了吗\r\n");
       // memcpy(ucComMF522Buf, pValue, 4);
        for (i=0; i<16; i++)
        {    
					ucComMF522Buf[i] = *(pValue+i);   
				}
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
				//printf("第二个\r\n");
        if (status != MI_ERR)
        {  
					status = MI_OK;    
				}
				//printf("第三个\r\n");
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
//			          printf("123456告诉我可以\r\n");
//		            }
//		            else
//			          printf("123456搞事啊\r\n");
		
					status = MI_ERR;   
				}
				//printf("第四个\r\n");
    }
		
		//printf("这里执行完毕\r\n");
    return status;

}
