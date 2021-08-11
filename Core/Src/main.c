/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FLASH_MAX_PAGE 256
#define	FLASH_BASE_ADDR 0X08000000
#define	FLASH_ERASE_ADDR 0X08003000//可能不是最终的擦除地址
//#define FLASH_PAGE_SIZE 2048
#define FLASH_ERASE_PAGE (FLASH_MAX_PAGE-((FLASH_ERASE_ADDR-FLASH_BASE_ADDR)/FLASH_PAGE_SIZE))//缺乏大小检测,小于2048怎么处理
#define FLASH_WRITE_VUALE 0x00000001

#define FLASH_ERASE_EN 1
#define FLASH_WRITE_EN 1
#define FLASH_READ_EN  1

#define LOOP 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t pageError = 0;
	uint32_t erase_adder = 0;
	uint32_t write_adder = 0;
	uint32_t erase_page = 0 ;
	uint32_t write_num = 0; //4字节写入次数
	uint32_t write_value = FLASH_WRITE_VUALE; //
	uint32_t i = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  SEGGER_RTT_ConfigUpBuffer(0,NULL,NULL,0,SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  SEGGER_RTT_printf(0,"flash test\r\n");
//	  HAL_Delay(1000);

	  SEGGER_RTT_printf(0,"flash test\r\n");
	  SEGGER_RTT_printf(0,"erase_adder:%x erase_page:%d\r\n ",FLASH_ERASE_ADDR,FLASH_ERASE_PAGE);
	  HAL_Delay(1000);
	  HAL_FLASH_Unlock();	//解锁flash
//	flash擦除
	  #if FLASH_ERASE_EN
		EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;			//擦除类型0x00	页擦除
		EraseInitStruct.PageAddress   = FLASH_ERASE_ADDR;					//起始地址
		EraseInitStruct.NbPages       = FLASH_ERASE_PAGE;					//页数
		if(HAL_FLASHEx_Erase(&EraseInitStruct, &pageError) != HAL_OK)	//启动擦除
		{
			SEGGER_RTT_printf(0,"adder:%x Erase Error\r\n",pageError);
		}
		else
		{
			SEGGER_RTT_printf(0,"adder:%x Erase Finish\r\n",pageError);
		}
		HAL_Delay(3000);
      #endif
//	flash写入
	  #if FLASH_WRITE_EN
		write_value = FLASH_WRITE_VUALE; //
		write_num = (FLASH_ERASE_PAGE*FLASH_PAGE_SIZE)/4;
		for(i = 0,write_adder = FLASH_ERASE_ADDR;i<write_num;i++)
		{
			__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR|FLASH_SR_EOP);
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,write_adder,write_value++) != HAL_OK)
			{
				SEGGER_RTT_printf(0,"adder:%x write error value:%x\r\n",write_adder,*(__IO uint32_t*)write_adder);
				break;
			}

//			else
//				SEGGER_RTT_printf(0,"adder:%x write:%x\r\n",write_adder,*(__IO uint32_t*)write_adder);
//			FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
			write_adder += 4;//地址增加4

//			uint16_t count = 10;
//			while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,write_adder,write_value)!=HAL_OK&&count--)
//			{
//				SEGGER_RTT_printf(0,"adder:%x write error value:%x\r\n",write_adder,*(__IO uint32_t*)write_adder);
//			}
//			if(count == 0)
//			{
//				SEGGER_RTT_printf(0,"adder:%x write error value:%x\r\n",write_adder,*(__IO uint32_t*)write_adder);
//				break;
//			}
//			write_value++;
//			write_adder += 4;//地址增加4
//			HAL_Delay(1);
//			uint16_t count=8000;
//			while(count--);

		}
		HAL_Delay(5000);
      #endif

//	读校验
	#if FLASH_READ_EN
	write_value = FLASH_WRITE_VUALE;
	write_num = (FLASH_ERASE_PAGE*FLASH_PAGE_SIZE)/4;
	for(i = 0,write_adder = FLASH_ERASE_ADDR;i<write_num;i++)
	{
		if(*(__IO uint32_t*)write_adder!=write_value++)
		{
			SEGGER_RTT_printf(0,"erro->adder:%x value:%x\r\n",write_adder,*(__IO uint32_t*)write_adder);
			break;
		}
		else
			SEGGER_RTT_printf(0,"adder:%x value:%x\r\n",write_adder,*(__IO uint32_t*)write_adder);
		write_adder += 4;//地址增加4
	}
	HAL_Delay(5000);
	#endif


	HAL_FLASH_Lock();	//上锁flash
	SEGGER_RTT_printf(0,"flash test over\r\n");

	#if LOOP
	HAL_Delay(5000);
	#else
	while(1);
	#endif

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
