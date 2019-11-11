/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/USB_Device/App/usb_device.c
  * @author  MCD Application Team
  * @brief   This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_hid.h"

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
extern void SystemClockConfig_Resume(void);

/* USER CODE END PFP */

static void Error_Handler(void) { while(1); }
/* USB Device Core handle declaration. */
extern USBD_DescriptorsTypeDef HID_Desc;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/**
  * @brief USB Clock Configuration
  * @retval None
  */
void USBD_Clock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_CRSInitTypeDef RCC_CRSInitStruct= {0};

  /* Enable HSI48 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    Error_Handler();
  }
  /*Configure the clock recovery system (CRS)**********************************/

  /*Enable CRS Clock*/
  __HAL_RCC_CRS_CLK_ENABLE();

  /* Default Synchro Signal division factor (not divided) */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;

  /* Set the SYNCSRC[1:0] bits according to CRS_Source value */
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;

  /* HSI48 is synchronized with USB SOF at 1KHz rate */
  RCC_CRSInitStruct.ReloadValue =  __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
  RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;

  /* Set the TRIM[5:0] to the default value */
  // RCC_CRSInitStruct.HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT;
  RCC_CRSInitStruct.HSI48CalibrationValue = 64;

  /* Start automatic synchronization */
  HAL_RCCEx_CRSConfig (&RCC_CRSInitStruct);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
