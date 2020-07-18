#include "motor.h"
#include "tim.h"

MotorSpeedControl MotorSpeedCtrl[4];//电机速度的控制结构，其中的pwm_width直接控制输出占空比，进而控制速度


/* Private variables ---------------------------------------------------------*/
// 平台控制硬件配置 -----------------------------------------
MotorCtrlHWConfig_t gMotor1_HWConfig = {
  .htim = &htim2,
  .ctrlPortPin = {{OUT_1_1_GPIO_Port, OUT_1_1_Pin, 0x00},         // 
                  //{IN_5_GPIO_Port, IN_5_Pin, 0x00},                 // I- hall sensor
                  },
};

MotorCtrlHWConfig_t gMotor2_HWConfig = {
  .htim = &htim3,
  .ctrlPortPin = {{OUT_1_2_GPIO_Port, OUT_1_2_Pin, 0x00},         // 
                 // {IN_6_GPIO_Port, IN_6_Pin, 0x00},                 // I- hall sensor
                },
};

MotorCtrlHWConfig_t gMotor3_HWConfig = {
  .htim = &htim4,
  .ctrlPortPin = {{OUT_1_3_GPIO_Port, OUT_1_3_Pin, 0x00},         // 
                 // {IN_7_GPIO_Port, IN_7_Pin, 0x00},                 // I- hall sensor
                },
};

MotorCtrlHWConfig_t gMotor4_HWConfig = {
  .htim = &htim5,
  .ctrlPortPin = {{OUT_1_4_GPIO_Port, OUT_1_4_Pin, 0x00},         // 
                 // {IN_8_GPIO_Port, IN_8_Pin, 0x00},                 // I- hall sensor
                },
};


void App_MotorSpeedCtrlService(int k, MotorCtrlHWConfig_t* gMotori_HWConfig)
{
	
	//pwm_control
	if(  MotorSpeedCtrl[k-1].pwm_cnt <= MotorSpeedCtrl[k-1].pwm_width)
	{    
		HAL_GPIO_WritePin(gMotori_HWConfig->ctrlPortPin[SERVO_PULSE].GPIOx, gMotori_HWConfig->ctrlPortPin[SERVO_PULSE].GPIO_Pin,GPIO_PIN_SET);
	}  
	else
	{
		HAL_GPIO_WritePin(gMotori_HWConfig->ctrlPortPin[SERVO_PULSE].GPIOx, gMotori_HWConfig->ctrlPortPin[SERVO_PULSE].GPIO_Pin,GPIO_PIN_RESET);
	}
	if(MotorSpeedCtrl[k-1].pwm_cnt>=100)
		MotorSpeedCtrl[k-1].pwm_cnt=0;
	MotorSpeedCtrl[k-1].pwm_cnt++;
  
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	if (htim->Instance == TIM2) 
		App_MotorSpeedCtrlService(1,&gMotor1_HWConfig);
	else if (htim->Instance == TIM3) 
		App_MotorSpeedCtrlService(2,&gMotor2_HWConfig);
	else if (htim->Instance == TIM4) 
		App_MotorSpeedCtrlService(3,&gMotor3_HWConfig);
	else if (htim->Instance == TIM5) 
		App_MotorSpeedCtrlService(4,&gMotor4_HWConfig);
	

}

