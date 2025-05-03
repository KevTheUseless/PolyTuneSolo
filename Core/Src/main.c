/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "arm_math.h"
#include "arm_const_structs.h"

#include "test_data.h"
#include "lookup.h"

#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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


#define TEST_LENGTH_SAMPLES 2048
#define SAMPLE_RATE 1024

extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float32_t testOutput[TEST_LENGTH_SAMPLES / 2];
uint32_t fftTestSize = 1024;

uint32_t adcBuffer[1];
volatile uint8_t adcFlag = 0;
float32_t voltage = 0.0;

#define LENGTH_SAMPLES 2048
static float32_t frame[LENGTH_SAMPLES];
static float32_t freqBins[LENGTH_SAMPLES / 2];
uint32_t fftSize = 1024;

arm_cfft_instance_f32 varInstCfftF32;

#define ABS(x) ((x) < 0 ? -(x) : (x))

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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  HAL_SPI_Init(&hspi2);
  HAL_TIM_Base_Start(&htim3);
  HAL_ADC_Start_DMA(&hadc1, adcBuffer, 1);
  
  arm_status status = ARM_MATH_SUCCESS;
  status = arm_cfft_init_1024_f32(&varInstCfftF32);
  
  /*
  arm_cfft_f32(&varInstCfftF32, testInput_f32_10khz, 0, 1);
  arm_cmplx_mag_f32(testInput_f32_10khz, testOutput, fftTestSize);
  
  for (int i = 0; i < fftTestSize / 2; i++)
  {
    printf("%.3f, ", testOutput[i]);
  }
  */
  
  uint16_t ptr = 0;
  
  char prev_note = 'A';
  char displayNote = 'A';
  uint8_t sharp = 0;
  
  LCD_Init();
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    printf("%f\n", voltage);
    
    if (adcFlag)
    {
      if (ptr == 2048)
        ptr = 0;
      adcFlag = 0;
    
      frame[ptr] = voltage;
      ptr += 2;
    }
    
    /*
    if (ptr == 2046)
      printf("\n----------\n");
    */
    
    if (ptr <= 256 || ptr % 512 != 0)
      continue;
    
    
    float32_t sum = 0.0;
    for (int i = 0; i < ptr / 2; i++)
    {
      if (frame[2 * i] == 0)
        break;
      sum += frame[2 * i];
    }
    float32_t avg = 0.0;
    for (int i = 0; i < ptr / 2; i++)
      frame[2 * i] -= avg;
    
    arm_cfft_f32(&varInstCfftF32, frame, 0, 1);
    arm_cmplx_mag_f32(frame, freqBins, fftSize);
    
    float32_t maxMag = 0.0;
    uint32_t maxIndex = 0;
    arm_max_f32(freqBins, fftSize, &maxMag, &maxIndex);
    float32_t maxFreq = maxIndex * 1000.0 / 1024.0;
    
    /*
    for (int i = 0; i < fftSize / 2; i++)
    {
      printf("%.3f,", freqBins[i]);
    }
    printf("\nmax freq: %fHz at %d\n", maxIndex * 1000.0 / 1024.0, maxIndex);
    */
    
    float32_t absDelta = 1000.0;
    float32_t delta = 1000.0;
    uint8_t closestNoteIndex = 0;
    for (int i = 0; i < FREQ_COUNT; i++)
    {
      float32_t temp = ABS(maxFreq - freqTable[i]);
      if (temp < absDelta)
      {
        absDelta = temp;
        delta = freqTable[i] - maxFreq;
        closestNoteIndex = i;
      }
    }
    char note = noteNameTable[closestNoteIndex % 11];
    
    // TODO: display
    if (note == prev_note)
      continue;
    else
    {
      displayNote = note;
      
      switch (note)
      {
        case '!':
          displayNote = 'A';
          sharp = 1;
          break;
        case '@':
          displayNote = 'C';
          sharp = 1;
          break;
        case '$':
          displayNote = 'D';
          sharp = 1;
          break;
        case '%':
          displayNote = 'G';
          sharp = 1;
          break;
        default:
          sharp = 0;
          break;
      }
      
      LCD_ShowChar(116, 30, BLUE, WHITE, displayNote, 16);
      if (sharp)
        LCD_ShowChar(130, 30, BLUE, WHITE, '#', 16);
      else
        LCD_ShowChar(130, 30, WHITE, WHITE, '#', 16);
      
      if (absDelta < 1)
      {
        LCD_ShowChar(116, 120, WHITE, GREEN, 'O', 16);
      }
      else
      {
        if (delta < 0)
          LCD_ShowChar(116, 120, RED, WHITE, 'H', 16);
        else
          LCD_ShowChar(116, 120, RED, WHITE, 'L', 16);
      }
    }
  } 
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
int fputc(int ch, FILE* f)
{
  HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 0xffff);
  return ch;
}

int fgetc(FILE* f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  adcFlag = 1;
  voltage = adcBuffer[0] * 3.3 / 4096.0;
  //HAL_ADC_Start_DMA(&hadc1, adcBuffer, 1);
}
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
