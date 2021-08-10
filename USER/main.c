#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "stmflash.h"
#include "SEGGER_RTT.h"

#define FLASH_MAX_PAGE 256
#define	FLASH_BASE_ADDR 0X08000000
#define	FLASH_ERASE_ADDR 0X08003000//���ܲ������յĲ�����ַ
#define FLASH_PAGE_SIZE 2048
#define FLASH_ERASE_PAGE (FLASH_MAX_PAGE-((FLASH_ERASE_ADDR-FLASH_BASE_ADDR)/FLASH_PAGE_SIZE))//ȱ����С���,С��2048��ô����
#define FLASH_WRITE_VUALE 0x00000001
/************************************************
 ALIENTEKս��STM32������ʵ��34
 FLASHģ��EEPROM ʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


//Ҫд�뵽STM32 FLASH���ַ�������
const u8 TEXT_Buffer[]={"STM32F103 FLASH TEST"};
#define SIZE sizeof(TEXT_Buffer)		//���鳤��
#define FLASH_SAVE_ADDR  0X08070000		//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)

 int main(void)
 {	 
	u8 key;
//	u16 i=0;
	u8 datatemp[SIZE];
	FLASH_Status f_status; 
	u32 erase_adder = 0; 
	u32 write_adder = 0;  
	u32 erase_page = 0 ;
	u32 write_num = 0; //4�ֽ�д�����
	 
	u32 write_value = FLASH_WRITE_VUALE; //	 
	 
	u32 t = 0;

	u32 i = 0;
	 
	 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
// 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
//	KEY_Init();					//��ʼ������
//	LCD_Init();			   		//��ʼ��LCD  
// 	POINT_COLOR=RED;			//��������Ϊ��ɫ 
//	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
//	LCD_ShowString(30,70,200,16,16,"FLASH EEPROM TEST");	
//	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,110,200,16,16,"2015/1/18"); 
//	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");
	 
	 
//	SEGGER_RTT_ConfigUpBuffer(0,NULL,NULL,0,SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
	 
	SEGGER_RTT_printf(0,"flash test\r\n");
	

//	__disable_irq();					//���ж�
	FLASH_Unlock();						//����
//	flash����
	for(i=0,erase_adder = FLASH_ERASE_ADDR;i<FLASH_ERASE_PAGE;i++)
	{
		if(FLASH_ErasePage(erase_adder)!= FLASH_COMPLETE)
		{
			SEGGER_RTT_printf(0,"erase erroe\r\n");
			break;
		}
		else
		{
			SEGGER_RTT_printf(0,"erase finish\r\n");
		}
		erase_adder += FLASH_PAGE_SIZE;		
	}
	
//	flashд��
	write_num = (FLASH_ERASE_PAGE*FLASH_PAGE_SIZE)/4;
	for(i = 0,write_adder = FLASH_ERASE_ADDR;i<write_num;i++)
	{
		if(FLASH_ProgramWord(write_adder,write_value++) != FLASH_COMPLETE)
		{
			SEGGER_RTT_printf(0,"adder:%x write error\r\n",write_adder);
		}
		else
			SEGGER_RTT_printf(0,"adder:%x write:%x\r\n",write_adder,*(vu32*)write_adder);
	    write_adder += 4;//��ַ����4
	} 
//	��У��
	write_value = FLASH_WRITE_VUALE;
	for(i = 0,write_adder = FLASH_ERASE_ADDR;i<write_num;i++)
	{
		if(*(vu32*)write_adder!=write_value++)
		{
			SEGGER_RTT_printf(0,"adder:%x value:%x\r\n",write_adder,*(vu32*)write_adder);
		}
		else
			SEGGER_RTT_printf(0,"adder:%x value:%x\r\n",write_adder,*(vu32*)write_adder);		
		write_adder += 4;//��ַ����4
	}
	
	FLASH_Lock();//����	

	SEGGER_RTT_printf(0,"test over\r\n");
	
//	__enable_irq();//���ж�  	
	
	
	while(1);
	

	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)	//KEY1����,д��STM32 FLASH
		{
			LCD_Fill(0,170,239,319,WHITE);//�������    
 			LCD_ShowString(30,170,200,16,16,"Start Write FLASH....");
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)TEXT_Buffer,SIZE);
			LCD_ShowString(30,170,200,16,16,"FLASH Write Finished!");//��ʾ�������
		}
		if(key==KEY0_PRES)	//KEY0����,��ȡ�ַ�������ʾ
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read FLASH.... ");
			STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:  ");//��ʾ�������
			LCD_ShowString(30,190,200,16,16,datatemp);//��ʾ�������ַ���
		}
		i++;
		delay_ms(10);  
		if(i==20)
		{
			LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}		   
	} 
}


