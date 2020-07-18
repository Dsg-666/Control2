/**
  ******************************************************************************
  * @file     : app_led.c
  * @author   : xiaofeng 
  * @version  : V1.0
  * @date     : 2019 07
  * @brief    : APP LED 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_LED_H__
#define __APP_LED_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/	 

/* Exported functions --------------------------------------------------------*/
void App_LEDServiceInit(void);
void App_LEDServiceFunc(void *_arg);

#ifdef __cplusplus
}
#endif

#endif 


/*****END OF FILE****/

