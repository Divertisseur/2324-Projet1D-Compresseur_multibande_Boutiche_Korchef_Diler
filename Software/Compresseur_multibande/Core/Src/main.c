/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "vu_meter.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "sgtl5000.h"
#include "RCFilter.h"
#include "i2c.h"
#include "sai.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define CLI_BUFFER_LENGTH 40
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t uart2_data;
static h_vu_t h_vu;
static char cli_buffer[CLI_BUFFER_LENGTH];
static uint32_t cli_it = 0;

uint16_t countStart = 0;
uint16_t countStop = 0;
uint16_t adc_val = 0;
double gain = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void sgt_5000init();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t Buffer_Src[48];
uint8_t Buffer_Dest[48];
h_RC_filter_t* filter;
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t *) ptr, (uint16_t)len, HAL_MAX_DELAY);

	return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)

{
	// Appui sur entrée = 13
	if (uart2_data != 13)
	{
		if (cli_it < CLI_BUFFER_LENGTH - 1)
			// - 1 parce qu'on laisse de la place pour le caractère '\0'
		{
			HAL_UART_Transmit(&huart2, &uart2_data, 1, HAL_MAX_DELAY);
			cli_buffer[cli_it] = uart2_data;
			cli_it++;
		}
	}
	else
	{
		cli_buffer[cli_it] = '\0';	// Caractère '\0' obligatoire pour bien terminer la chaine de caractères
		printf("\r\n:%s\r\n", cli_buffer);

		switch(cli_buffer[0])
		{
		case 'v':
			cli_vu_meter(cli_buffer);
			break;
		case 'g':
			char gainString[sizeof(cli_buffer) - 2];
			int i;
			for (i = 2; i < sizeof(cli_buffer); i++) {
			        gainString[i - 2] = cli_buffer[i];
			    }
			char* errCheck;
			gain = strtod(gainString, &errCheck);
			printf("Changement du gain : %f\n", gain);

		default:
			printf("Unknown command\r\n");
			break;
		}

		cli_it = 0;	// Pour redémarrer à 0
		printf("> ");
		fflush(stdout);
		for (int i = 0 ; i < CLI_BUFFER_LENGTH ; i++)
		{
			// Vider le buffer pour plus de sécurité
			cli_buffer[i] = 0;
		}
	}

	HAL_UART_Receive_IT(&huart2, &uart2_data, 1);
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai){


}
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai){
	printf("Interruption declenchee ! \r\n");
	int i;
	for (i=0; i<48; i++){
		Buffer_Dest[i] = RC_filter_update(filter, Buffer_Src[i]);
	}

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI3_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_DAC1_Init();
  MX_I2C2_Init();
  MX_SAI2_Init();
  /* USER CODE BEGIN 2 */
	printf("\r\n=============================================================\r\n");
	printf("> ");
	HAL_StatusTypeDef statusUart;
	statusUart = HAL_UART_Receive_IT(&huart2, &uart2_data, 1);
	printf("Status de l'interruption UART : %d\r\n", statusUart);
	HAL_TIM_Base_Start(&htim6);
	__HAL_SAI_ENABLE(&hsai_BlockA2);

	printf("Demarrage des interruptions DMA \r\n.");
	HAL_SAI_Receive_DMA(&hsai_BlockA2, Buffer_Src, 48);
	HAL_SAI_Receive_DMA(&hsai_BlockB2, Buffer_Src, 48);
	HAL_SAI_Transmit_DMA(&hsai_BlockA2, Buffer_Dest, 48);
	HAL_SAI_Transmit_DMA(&hsai_BlockB2, Buffer_Dest, 48);
	printf("Interruptions demarrees.\r\n");
	// Configuration du CODEC
	printf("Initialisation du codec\r\n");
	int check;
	check = sgtl_5000init();
	if(check==0){
		printf("Initialisation OK\r\n");
	}
	else{
		printf("Erreur dans l'initialisation du codec\r\n");
	}
	// Initialisation d'un filtre
	RC_filter_init(filter, 500, 48000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{



#ifdef UART_POLLING
		uint8_t valeur;
		HAL_UART_Receive(&huart2, &valeur, 1, 1000);
		HAL_UART_Transmit(&huart2, &valeur, 1, HAL_MAX_DELAY);
#endif //UART_POLLING

#ifdef LED
		if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
		{
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			HAL_Delay(100);	// 100ms : 10Hz
		}
		else
		{
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		}
#endif //LED
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		// Ou là !
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI2|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 13;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV17;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV8;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK|RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

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
