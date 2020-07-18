/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void User_1(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OUT_1_7_Pin GPIO_PIN_5
#define OUT_1_7_GPIO_Port GPIOE
#define W5500_NINT_Pin GPIO_PIN_2
#define W5500_NINT_GPIO_Port GPIOA
#define W5500_NRST_Pin GPIO_PIN_3
#define W5500_NRST_GPIO_Port GPIOA
#define W5500_NSS_Pin GPIO_PIN_4
#define W5500_NSS_GPIO_Port GPIOA
#define W5500_SCK_Pin GPIO_PIN_5
#define W5500_SCK_GPIO_Port GPIOA
#define W5500_MISO_Pin GPIO_PIN_6
#define W5500_MISO_GPIO_Port GPIOA
#define W5500_MOSI_Pin GPIO_PIN_7
#define W5500_MOSI_GPIO_Port GPIOA
#define RED_LED_Pin GPIO_PIN_10
#define RED_LED_GPIO_Port GPIOE
#define W25Q_FLASH_CS_Pin GPIO_PIN_12
#define W25Q_FLASH_CS_GPIO_Port GPIOB
#define OUT_1_1_Pin GPIO_PIN_12
#define OUT_1_1_GPIO_Port GPIOG
#define OUT_1_2_Pin GPIO_PIN_13
#define OUT_1_2_GPIO_Port GPIOG
#define OUT_1_3_Pin GPIO_PIN_14
#define OUT_1_3_GPIO_Port GPIOG
#define OUT_1_4_Pin GPIO_PIN_15
#define OUT_1_4_GPIO_Port GPIOG
#define OUT_1_5_Pin GPIO_PIN_4
#define OUT_1_5_GPIO_Port GPIOB
#define OUT_1_6_Pin GPIO_PIN_7
#define OUT_1_6_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
