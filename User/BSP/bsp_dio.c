/**
  ******************************************************************************
  * @file           : bsp_dio.c
  * @brief          : 数字 IO 控制
  ******************************************************************************
  */

/* Private includes ----------------------------------------------------------*/
#include "bsp_dio.h"

#include "main.h"
#include "tim.h"
/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  DIO init structure definition
  */
typedef struct
{
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
  uint8_t logic;        // 0x0:ON-GPIO_PIN_SET,OFF-GPIO_PIN_RESET; 0x1:ON-GPIO_PIN_RESET,OFF-GPIO_PIN_SET;
}DIO_InitType_t;
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static DIO_InitType_t stType[DIO_MAX_TYPE];
/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  DIO 初始化
  * @note   使用 DIO 必须调用这个函数，否则会导致操作结果异常
  * @param  None
  * @retval None
  */
void DIO_Init(void)
{
  DIO_CameraLED_SetLight(DIO_CAMLED_1, 0x0);
  DIO_CameraLED_SetLight(DIO_CAMLED_2, 0x0);
  
  stType[DIO_3CLED_R].GPIOx = LED_R_GPIO_Port;
  stType[DIO_3CLED_R].GPIO_Pin = LED_R_Pin;
  stType[DIO_3CLED_R].logic = 0x1;

  stType[DIO_3CLED_G].GPIOx = LED_G_GPIO_Port;
  stType[DIO_3CLED_G].GPIO_Pin = LED_G_Pin;
  stType[DIO_3CLED_G].logic = 0x1;

  stType[DIO_3CLED_B].GPIOx = LED_B_GPIO_Port;
  stType[DIO_3CLED_B].GPIO_Pin = LED_B_Pin;
  stType[DIO_3CLED_B].logic = 0x1;

  stType[DIO_BREAK_1].GPIOx = BREAK_1_GPIO_Port;
  stType[DIO_BREAK_1].GPIO_Pin = BREAK_1_Pin;
  stType[DIO_BREAK_1].logic = 0x1;
  
  stType[DIO_BREAK_2].GPIOx = BREAK_2_GPIO_Port;
  stType[DIO_BREAK_2].GPIO_Pin = BREAK_2_Pin;
  stType[DIO_BREAK_2].logic = 0x1;
  
//  stType[DIO_CAMLED_1].GPIOx = CAMERA_LED_1_GPIO_Port;
//  stType[DIO_CAMLED_1].GPIO_Pin = CAMERA_LED_1_Pin;
//  stType[DIO_CAMLED_1].logic = 0x0;
//  
//  stType[DIO_CAMLED_2].GPIOx = CAMERA_LED_2_GPIO_Port;
//  stType[DIO_CAMLED_2].GPIO_Pin = CAMERA_LED_2_Pin;
//  stType[DIO_CAMLED_2].logic = 0x0;
}

/**
  * @brief  DIO 控制
  * @note   Node
  * @param  _Type,_Mode
  * @retval None
  */
void DIO_Ctrl(DIO_Type _Type, DIO_CtrlMode _Mode)
{
  if(  stType[_Type].GPIOx==0x0 && stType[DIO_CAMLED_1].GPIO_Pin==0x0)
  {
    return;
  }
  
  if( stType[_Type].logic == 0x0 )
  {
    switch(_Mode)
    {
      case DIO_ON:
        HAL_GPIO_WritePin(stType[_Type].GPIOx, stType[_Type].GPIO_Pin, GPIO_PIN_SET);
        break;
      case DIO_OFF:
        HAL_GPIO_WritePin(stType[_Type].GPIOx, stType[_Type].GPIO_Pin, GPIO_PIN_RESET);
        break;
      case DIO_TOOGLE:
        HAL_GPIO_TogglePin(stType[_Type].GPIOx, stType[_Type].GPIO_Pin);
        break;
      default:
        break;
    }
  }
  else
  {
    switch(_Mode)
    {
      case DIO_ON:
        HAL_GPIO_WritePin(stType[_Type].GPIOx, stType[_Type].GPIO_Pin, GPIO_PIN_RESET);
        break;
      case DIO_OFF:
        HAL_GPIO_WritePin(stType[_Type].GPIOx, stType[_Type].GPIO_Pin, GPIO_PIN_SET);
        break;
      case DIO_TOOGLE:
        HAL_GPIO_TogglePin(stType[_Type].GPIOx, stType[_Type].GPIO_Pin);
        break;
      default:
        break;
    }
  }
}

/**
  * @brief  DIO 读状态
  * @note   Node
  * @param  _Type,_Mode
  * @retval None
  */
DIO_PinState DIO_ReadPin(DIO_Type _Type)
{
  GPIO_PinState bitstatus = HAL_GPIO_ReadPin(stType[_Type].GPIOx, stType[_Type].GPIO_Pin);
  
  if(bitstatus == GPIO_PIN_SET)
  {
    return (DIO_PIN_SET);
  }
  
  return (DIO_PIN_RESET);
}

/**
  * @brief  气阀控制
  * @note   None
  * @param  _Mode ON-吸附, OFF - 关闭
  * @retval None
  */
void DIO_ValveCtrl(DIO_CtrlMode _Mode)
{
  switch(_Mode)
  {
    case DIO_ON:// 吸附
      DIO_Ctrl(DIO_BREAK_1, DIO_ON);
      DIO_Ctrl(DIO_BREAK_2, DIO_OFF);
      break;
    case DIO_OFF:// 关闭工作
      DIO_Ctrl(DIO_BREAK_1, DIO_OFF);
      DIO_Ctrl(DIO_BREAK_2, DIO_OFF);
      break;
    default:
      break;
  }

}

/**
  * @brief  补光灯亮度控制
  * @note   None
  * @param  None
  * @retval None
  */
void DIO_CameraLED_SetLight(DIO_Type _Type, uint8_t light)
{
  if( light > 100 )
  {
    light = 100;
  }
  
  if( _Type==DIO_CAMLED_1 )
  {
    TIM1_PWM_SetValue(TIM_CHANNEL_3, light);
  }
  
  if( _Type==DIO_CAMLED_2 )
  {
    TIM1_PWM_SetValue(TIM_CHANNEL_4, light);
  }
  
}

/********END OF FILE****/
