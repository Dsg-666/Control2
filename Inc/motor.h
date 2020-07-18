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

// �������Ӳ������
typedef struct 
{
  TIM_HandleTypeDef *htim;              // ��ʱ��
  IO_PortPin_t ctrlPortPin[0x02];    // ���� IO �ܽŶ��� {�ŷ����壬�����ź�}
}MotorCtrlHWConfig_t;

typedef struct
{
  int8_t pwm_width;             //ռ�ձ�             
  uint8_t pwm_cnt;              //pwm��������100Ϊ���ڣ���������n�����Ϊ�ߣ�100-n�����Ϊ��
 
}MotorSpeedControl;             //������ƽṹ�壬Ŀǰʹ�õ��Ƿ��ҵ��

/* Exported constants --------------------------------------------------------*/
extern MotorCtrlHWConfig_t gMotor1_HWConfig;
extern MotorCtrlHWConfig_t gMotor2_HWConfig;
extern MotorCtrlHWConfig_t gMotor3_HWConfig;
extern MotorCtrlHWConfig_t gMotor4_HWConfig;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


#endif



