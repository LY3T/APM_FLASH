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
#define	FLASH_ERASE_ADDR 0X08003000//可能不是最终的擦除地址
#define FLASH_PAGE_SIZE 2048
#define FLASH_ERASE_PAGE (FLASH_MAX_PAGE-((FLASH_ERASE_ADDR-FLASH_BASE_ADDR)/FLASH_PAGE_SIZE))//缺乏大小检测,小于2048怎么处理
#define FLASH_WRITE_VUALE 0x00000001
/************************************************
 ALIENTEK战舰STM32开发板实验34
 FLASH模拟EEPROM 实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


//要写入到STM32 FLASH的字符串数组
const u8 TEXT_Buffer[]={"STM32F103 FLASH TEST"};
#define SIZE sizeof(TEXT_Buffer)		//数组长度
#define FLASH_SAVE_ADDR  0X08070000		//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

 int main(void)
 {	 
	u8 key;
//	u16 i=0;
	u8 datatemp[SIZE];
	FLASH_Status f_status; 
	u32 erase_adder = 0; 
	u32 write_adder = 0;  
	u32 erase_page = 0 ;
	u32 write_num = 0; //4字节写入次数
	 
	u32 write_value = FLASH_WRITE_VUALE; //	 
	 
	u32 t = 0;

	u32 i = 0;
	 
	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
//	uart_init(115200);	 	//串口初始化为115200
// 	LED_Init();		  			//初始化与LED连接的硬件接口
//	KEY_Init();					//初始化按键
//	LCD_Init();			   		//初始化LCD  
// 	POINT_COLOR=RED;			//设置字体为红色 
//	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
//	LCD_ShowString(30,70,200,16,16,"FLASH EEPROM TEST");	
//	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,110,200,16,16,"2015/1/18"); 
//	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");
	 
	 
//	SEGGER_RTT_ConfigUpBuffer(0,NULL,NULL,0,SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
	 
	SEGGER_RTT_printf(0,"flash test\r\n");
	

//	__disable_irq();					//关中断
	FLASH_Unlock();						//解锁
//	flash擦除
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
	
//	flash写入
	write_num = (FLASH_ERASE_PAGE*FLASH_PAGE_SIZE)/4;
	for(i = 0,write_adder = FLASH_ERASE_ADDR;i<write_num;i++)
	{
		if(FLASH_ProgramWord(write_adder,write_value++) != FLASH_COMPLETE)
		{
			SEGGER_RTT_printf(0,"adder:%x write error\r\n",write_adder);
		}
		else
			SEGGER_RTT_printf(0,"adder:%x write:%x\r\n",write_adder,*(vu32*)write_adder);
	    write_adder += 4;//地址增加4
	} 
//	读校验
	write_value = FLASH_WRITE_VUALE;
	for(i = 0,write_adder = FLASH_ERASE_ADDR;i<write_num;i++)
	{
		if(*(vu32*)write_adder!=write_value++)
		{
			SEGGER_RTT_printf(0,"adder:%x value:%x\r\n",write_adder,*(vu32*)write_adder);
		}
		else
			SEGGER_RTT_printf(0,"adder:%x value:%x\r\n",write_adder,*(vu32*)write_adder);		
		write_adder += 4;//地址增加4
	}
	
	FLASH_Lock();//上锁	

	SEGGER_RTT_printf(0,"test over\r\n");
	
//	__enable_irq();//开中断  	
	
	
	while(1);
	

	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)	//KEY1按下,写入STM32 FLASH
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(30,170,200,16,16,"Start Write FLASH....");
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)TEXT_Buffer,SIZE);
			LCD_ShowString(30,170,200,16,16,"FLASH Write Finished!");//提示传送完成
		}
		if(key==KEY0_PRES)	//KEY0按下,读取字符串并显示
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read FLASH.... ");
			STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:  ");//提示传送完成
			LCD_ShowString(30,190,200,16,16,datatemp);//显示读到的字符串
		}
		i++;
		delay_ms(10);  
		if(i==20)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}		   
	} 
}


