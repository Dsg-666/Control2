/**
  ******************************************************************************
  * @file           : bsp_dio.h
  * @brief          : 数字 IO 控制
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_DIO_H__
#define __BSP_DIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/** 
  * @brief  DIO 类型
  */  
typedef enum 
{
  DIO_3CLED_R,
  DIO_3CLED_G,
  DIO_3CLED_B,
  DIO_BREAK_1,
  DIO_BREAK_2,
  DIO_CAMLED_1,
  DIO_CAMLED_2,
  
  DIO_MAX_TYPE,        // DIO 最大数量
} DIO_Type;

/** 
  * @brief  DIO 控制类型
  */  
typedef enum 
{
  DIO_ON,
  DIO_OFF,
  DIO_TOOGLE,
} DIO_CtrlMode;

/** 
  * @brief  DIO 管脚状态
  */  
typedef enum
{
  DIO_PIN_RESET = 0,
  DIO_PIN_SET
}DIO_PinState;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void DIO_Init(void);

DIO_PinState DIO_ReadPin(DIO_Type _Type);
void DIO_Ctrl(DIO_Type _Type, DIO_CtrlMode _Mode);

void DIO_ValveCtrl(DIO_CtrlMode _Mode);
void DIO_CameraLED_SetLight(DIO_Type _Type, uint8_t light);

/* Exported defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
