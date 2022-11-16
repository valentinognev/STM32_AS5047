/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.c
  * @brief   This file provides code for the configuration
  *          of all the requested memory to memory DMA transfers.
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
#include "dma.h"

/* USER CODE BEGIN 0 */
#include "stm32f4xx_hal_gpio.h"
#include "main.h"

uint8_t spiTxFinished = 1;
uint8_t spiRxFinished = 1;
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Enable DMA controller clock
  */
void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 1));
  NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 1));
  NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/* USER CODE BEGIN 2 */
void SPI_TransmitReceive_DMA(uint16_t* transferData, uint16_t* receiveData, uint16_t size)
{
	for (uint16_t i=0; i<size; i++)
	{
		LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
		LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_5);
		LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_0, LL_SPI_DMA_GetRegAddr(SPI3), (uint32_t)(receiveData+i), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_0));
		LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_5, (uint32_t)(transferData+i), LL_SPI_DMA_GetRegAddr(SPI3), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_5));

		HAL_GPIO_WritePin(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, GPIO_PIN_RESET); spiTxFinished = 0;spiRxFinished = 0;
		LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
		LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_5);
	}
}

void SPI_Transfer_DMA(uint16_t* transferData, uint16_t size)
{
	for (uint16_t i=0; i<size; i++)
	{
		LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_5);
		LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_5, (uint32_t)(transferData+i), LL_SPI_DMA_GetRegAddr(SPI3), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_5));

		HAL_GPIO_WritePin(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, GPIO_PIN_RESET); spiTxFinished = 0;
		LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_5);
	}
}

void SPI_Receive_DMA(uint16_t* receiveData, uint16_t size)
{
	for (uint16_t i=0; i<size; i++)
	{
		LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
		LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_0, LL_SPI_DMA_GetRegAddr(SPI3), (uint32_t)(receiveData+i), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_0));

		HAL_GPIO_WritePin(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, GPIO_PIN_RESET); spiRxFinished = 0;
		LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
	}
}

void DMA1_Stream0_TransferComplete(void)
{
    // TX Done .. Do Something ...
	LL_DMA_ClearFlag_TC0(DMA1);
	if (spiTxFinished)
	  HAL_GPIO_WritePin(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, GPIO_PIN_SET);
	spiRxFinished = 1;
}
void DMA1_Stream5_TransferComplete(void)
{
    // RX Done .. Do Something ...
	LL_DMA_ClearFlag_TC5(DMA1);
	if (spiRxFinished)
	  HAL_GPIO_WritePin(AS5047D_CS1_GPIO_Port, AS5047D_CS1_Pin, GPIO_PIN_SET);
	spiTxFinished = 1;
}
/* USER CODE END 2 */

