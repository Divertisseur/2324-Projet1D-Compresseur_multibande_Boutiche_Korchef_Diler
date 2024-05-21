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
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "RCFilter.h"
#include <string.h>
#include "sgtl5000.h"
#include "kiss_fftr.h"
#include "oledprint.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SAI_TX_BUFFER_LENGTH 128
#define SAI_RX_BUFFER_LENGTH 128
#define N_FFT 128  // Number of FFT points
#define OUTPUT_MAX 64.0f  // Desired maximum output value
#define UCHAR_MAX 255.0f  // Maximum value for unsigned char
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t uart2_data;
volatile uint16_t adcResultsDMA[13];
const int adcChannelCount = sizeof(adcResultsDMA) / sizeof(adcResultsDMA[0]);
volatile int adcConversionComplete = 0;
uint8_t textLenght;
h_RC_filter_t HPF;
h_RC_filter_t LPF;
static int16_t sai_tx_buffer[SAI_TX_BUFFER_LENGTH];
static int16_t sai_rx_buffer[SAI_RX_BUFFER_LENGTH];
unsigned char * FFT_array;
int CutoffFreq;
float gain_input;
float gain_output;
unsigned char output_magnitude[N_FFT/2 + 1];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

adcConversionComplete = 1;

}
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)ptr, (uint16_t)len, HAL_MAX_DELAY);
	return len;
}
// Interruptions
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* uart){
	HAL_UART_Transmit(&huart2, &uart2_data, 1, HAL_MAX_DELAY);
	HAL_UART_Receive_IT(&huart2, &uart2_data, 1);
}
// Réception de données sur le SAI
static int rx_cplt_flag = 0;

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
	//printf("interruption sai\n");
	if (SAI2_Block_B == hsai->Instance)
	{
		rx_cplt_flag = 1;
	}
}
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai){
	//printf("RX half callback.\n");
}
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
	if (SAI2_Block_A == hsai->Instance)
	{
		//printf("SAI interrupt TX.\n");
	}
}
void compute_fft(const int16_t *input, unsigned char *output_magnitude) {
    kiss_fftr_cfg cfg = kiss_fftr_alloc(N_FFT, 0, NULL, NULL);

    if (!cfg) {
        printf("Not enough memory for FFT configuration\n");
        return;
    }

    // Allocate memory for input and output
    kiss_fft_scalar in[N_FFT];
    kiss_fft_cpx out[N_FFT/2 + 1];

    // Convert int16_t input to kiss_fft_scalar (float)
    for (int i = 0; i < N_FFT; i++) {
        in[i] = (kiss_fft_scalar)input[i];
    }

    // Perform the FFT
    kiss_fftr(cfg, in, out);

    // Calculate the magnitude of the FFT
    float max_magnitude = 0.0f;
    float temp_magnitude[N_FFT/2 + 1];
    for (int i = 0; i <= N_FFT/2; i++) {
        temp_magnitude[i] = sqrtf(out[i].r * out[i].r + out[i].i * out[i].i);
        if (temp_magnitude[i] > max_magnitude) {
            max_magnitude = temp_magnitude[i];
        }
    }

    // Normalize and scale the magnitudes to the range 0 to 255
    if (max_magnitude > 0.0f) {
        for (int i = 0; i <= N_FFT/2; i++) {
            // Normalize to 0 to 64
            float normalized = (temp_magnitude[i] / max_magnitude) * OUTPUT_MAX;
            // Scale to 0 to 255
            output_magnitude[i] = (unsigned char)((normalized / OUTPUT_MAX) * UCHAR_MAX);
        }
    } else {
        // If max_magnitude is zero, set all outputs to zero
        for (int i = 0; i <= N_FFT/2; i++) {
            output_magnitude[i] = 0;
        }
    }

    // Free the FFT configuration
    free(cfg);
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
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_SAI2_Init();
  MX_I2C2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, &uart2_data, 1);
  printf("\n-----------------------------------\nDemarrage du programme.\r\n");
  printf("Initialisation du SAI...\r\n");
  __HAL_SAI_ENABLE(&hsai_BlockA2);
  __HAL_SAI_ENABLE(&hsai_BlockB2);
  //printf("Initalisation de l'afficheur OLED...\r\n");
  //initOled();
  printf("Initialisation du codec\r\n");

  HAL_StatusTypeDef ret;
  uint16_t data;
  uint16_t sgtl_address = 0x14;

  h_sgtl5000_t h_sgtl5000;
  h_sgtl5000.hi2c = &hi2c2;
  h_sgtl5000.dev_address = sgtl_address;

  ret = sgtl5000_init(&h_sgtl5000);
  if (ret != HAL_OK){
	  printf("SGTL INIT ERROR. \n");
	  Error_Handler();
  }
  ret = sgtl5000_i2c_read_register(&h_sgtl5000, SGTL5000_CHIP_ID, &data);

  if (ret != HAL_OK)
  	{
  		printf("HAL_I2C_Mem_Read error\n");
  		Error_Handler();
  	}
//  for (int i = 0 ; i < SAI_TX_BUFFER_LENGTH ; i++)
//  	{
//  		// Generate a sawtooth at 1kHz
//  		sai_tx_buffer[i] = i * (0xFFFF/SAI_TX_BUFFER_LENGTH);
//  	}
  ret = HAL_SAI_Receive_DMA(&hsai_BlockB2, (uint8_t*) sai_rx_buffer, SAI_RX_BUFFER_LENGTH);
  if(ret !=HAL_OK){
	  printf("Failed to start DMA on SAI Receive\n");
  }
  ret = HAL_SAI_Transmit_DMA(&hsai_BlockA2, (uint8_t*) sai_tx_buffer, SAI_TX_BUFFER_LENGTH);
  if(ret !=HAL_OK){
	  printf("Failed to start DMA on SAI Transmit\n");
  }
//  HAL_SAI_DisableRxMuteMode(&hsai_BlockA2);
  printf("SAI OK, initialisation du filtre...\n");
// Démarrage des filtres
  HPF_init(&HPF, 1000, 48000);
  LPF_init(&LPF, 1000, 48000);
  HAL_TIM_Base_Start(&htim1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcResultsDMA, adcChannelCount);
	  HAL_SAI_Receive_DMA(&hsai_BlockA2, (uint8_t*) sai_rx_buffer, SAI_RX_BUFFER_LENGTH);

	  while(adcConversionComplete  == 0){

		 	  }
	  adcConversionComplete = 0;
	  int i;
	  for (i=1; i<2; i++){
		  printf("Channel %d : %d\r\n", i, adcResultsDMA[i]);

		  HAL_Delay(100);
	  }
	  //printf("Audio buffer (RX) first value : %d\n\r", sai_rx_buffer[0]);
	  CutoffFreq = roundf(((adcResultsDMA[3] + 50)/4));
	  gain_input = adcResultsDMA[0]/4096; // Les potards varient de 0 à 4096 et on veut un gain entre 0 et 1
	  gain_output = adcResultsDMA[1]/4096;
	  //printf("gain output : %d\n", gain_output);
	  LPF_init(&LPF, CutoffFreq, 48000);
	  HPF_init(&HPF, CutoffFreq, 48000);
	  if (rx_cplt_flag == 1){
			for (int i = 0 ; i < SAI_RX_BUFFER_LENGTH ; i++)
			{
				// Gain en entrée
				sai_rx_buffer[i] = sai_rx_buffer[i]*gain_input;
				// Filtre
				sai_tx_buffer[i] = (uint16_t)HPF_update(&HPF, sai_rx_buffer[i]);
				sai_tx_buffer[i] = (uint16_t)LPF_update(&LPF, sai_tx_buffer[i]);
				// Gain en sortie
				sai_tx_buffer[i] = sai_tx_buffer[i]*gain_output;
			}
	  }
	  // On fait la FFT
	  compute_fft(sai_tx_buffer, output_magnitude);
	  printSpect(output_magnitude);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	 HAL_Delay(100);
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
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
  PeriphClkInit.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 11;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
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
