#ifndef __MOTOR_H
#define __MOTOR_H
	
#include "main.h"


typedef enum
{
  SERVO_PULSE, 	//0
  SERVO_HALL, 	//1
}PlateHWPortPinOffset_t;

/**
  * @brief  DIO init structure definition
  */
typedef struct
{
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
  uint8_t logic;        // 0x0:ON-GPIO_PIN_SET,OFF-GPIO_PIN_RESET; 0x1:ON-GPIO_PIN_RESET,OFF-GPIO_PIN_SET;
}IO_PortPin_t;

// 电机控制硬件配置
typedef struct 
{
  TIM_HandleTypeDef *htim;              // 定时器
  IO_PortPin_t ctrlPortPin[0x02];    // 控制 IO 管脚定义 {伺服脉冲，霍尔信号}
}MotorCtrlHWConfig_t;

typedef struct
{
  int8_t pwm_width;             //占空比             
  uint8_t pwm_cnt;              //pwm计数，以100为周期，控制其中n次输出为高，100-n次输出为低
 
}MotorSpeedControl;             //电机控制结构体，目前使用的是缝纫电机

/* Exported constants --------------------------------------------------------*/
extern MotorCtrlHWConfig_t gMotor1_HWConfig;
extern MotorCtrlHWConfig_t gMotor2_HWConfig;
extern MotorCtrlHWConfig_t gMotor3_HWConfig;
extern MotorCtrlHWConfig_t gMotor4_HWConfig;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


#endif



