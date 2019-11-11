/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.h
  * @version        : v3.0_Cube
  * @brief          : Header for usb_device.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "usbd_def.h"

void USBD_Clock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_DEVICE_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
