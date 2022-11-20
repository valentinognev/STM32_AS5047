/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AS5047D.h"
#include "debug_scope.h"
#include "usbd_cdc_if.h"
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
DebugCommand debugCommand = 0;
volatile uint8_t timTrig = 0;
DebugScope_Handle_t debugData =
	{
        .sz = DEBUGSCOPESIZE,
        .curCh = 1,
        .i1 = 0,
        .i2 = 0,
        .i3 = 0,
        .i4 = 0,
        .i5 = 0,
        .startWriteFlag = false
    };
int32_t encoderAngle = 0;
float spiAngle = 0;

uint16_t ERRFL = 0;
uint16_t PROG = 0;
uint16_t DIAAGC = 0;
uint16_t ANGLEUNC = 0;

uint16_t NOP = 0;
uint16_t CORDICMAG = 0;
uint16_t ANGLECOM = 0;
uint16_t ZPOSM = 0;
uint16_t ZPOSL = 0;
uint16_t SETTINGS1 = 0;
uint16_t SETTINGS2 = 0;

int32_t timerData = 0;
float true_angle = 0.0f;

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
  MX_SPI1_Init();
  MX_TIM9_Init();
  MX_SPI3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  LL_TIM_EnableCounter(TIM4);
  /* Initiaize AS5047D */
  uint16_t nop,AGC;

  uint8_t errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_NOP, &nop);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Get_AGC_Value(&AGC);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_PROG, &PROG);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ANGLEUNC, &ANGLEUNC);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_CORDICMAG, &CORDICMAG);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ANGLECOM, &ANGLECOM);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ZPOSM, &ZPOSM);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ZPOSL, &ZPOSL);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_SETTINGS1, &SETTINGS1);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
  errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_SETTINGS2, &SETTINGS2);
  if (errorFlag != 0)    errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);

  errorFlag = AS5047D_Get_True_Angle_Value(&true_angle);
    //
    //
    //  AS5047D_Init();
    //  AS5047D_SetZero();

    LL_TIM_EnableCounter(TIM9);
    // HAL_TIM_Base_Start(&htim9);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    char buf[10] = {"my test"};
    char inData[10] = "";
    uint32_t inDataSZ = strlen(inData);
    while (1)
    {
      while (!timTrig)
        ;

      errorFlag = AS5047D_Get_True_Angle_Value(&spiAngle);
      if (errorFlag != 0)
      {
        errorFlag = AS5047D_Read(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, AS5047D_ERRFL, &ERRFL);
        errorFlag++;
      }
      timTrig = 0;
      //
      DebugScopeInsertData(&debugData, 2, (int)(spiAngle*50));

      // CDC_Transmit_FS(buf, strlen(buf));
      if (VCP_retrieveInputData(inData, &inDataSZ))
        if (strlen(inData) > 0)
        {
          debugCommand = inData[0]; // atoi(buf);
          if (debugCommand == START_WRITE)
            DebugScopeStartWrite(&debugData);
          else if (debugCommand == TRANSMIT_DATA)
            CDC_Transmit_FS((uint8_t*)(debugData.Ch1), sizeof(debugData.Ch1) * 2);
        }

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == START_WRITE_Pin)
	{
		debugData.startWriteFlag = true;
	}
	else if (GPIO_Pin == GET_DBG_DATA_Pin)
	{
		HAL_UART_Transmit (&huart2, debugData.Ch1, sizeof (debugData.Ch1)*2, 10);
	}
}
void tim9eventFunction()
{
  DebugScopeInsertData(&debugData, 1, encoderAngle);
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
