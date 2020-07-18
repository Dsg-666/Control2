/**
  ******************************************************************************
  * @file           : mdw_servo.h
  * @brief          : Servo Driving 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MDW_SERVO_CTRL_H__
#define __MDW_SERVO_CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  int32_t encValue;           // ����������
  int32_t encOffsetValue;     // ����������ƫ��
  uint32_t stateCode;         // ״̬��
  
  uint16_t gearRatio ;         // ���ٱ�
}Servo_CtrlData_t;

/* Exported defines ----------------------------------------------------------*/
#define SERVO_SUPPORT_TOP_LIFT_MOTOR          (0x00)    // ֧�ֶ��� �������
#define SERVO_SUPPORT_CAN_SEND_MUTEX          (0x00)    // ֧�ַ��ͻ�����

#define SERVO_LEFT_NODE_ID                    (0x01)
#define SERVO_RIGHT_NODE_ID                   (0x02)
#define SERVO_TOP_NODE_ID                     (0x04)
#define SERVO_LIFT_NODE_ID                    (0x05)
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

extern Servo_CtrlData_t gLeftServoData;
extern Servo_CtrlData_t gRightServoData;

#if (SERVO_SUPPORT_TOP_LIFT_MOTOR > 0x0)
extern Servo_CtrlData_t gTopServoData;
extern Servo_CtrlData_t gLiftServoData;
#endif // SERVO_SUPPORT_TOP_LIFT_MOTOR

/* Exported functions prototypes ---------------------------------------------*/
void Servo_InitConfig(void);

void Servo_MotorEnable(uint8_t node);
void Servo_MotorDisable(uint8_t node);
void Servo_ClearEncoderCount(uint8_t node);
void Servo_MotorBreakStop(uint8_t node,uint8_t cmd);
void Servo_MotorEmergencyStop(uint8_t node,uint8_t cmd);
void Servo_SetAutoReportContent(uint8_t node, int16_t Content1, int16_t Content2);
 
uint8_t Servo_SetMotorSpeed(uint8_t node, int16_t speed);

uint8_t Servo_SetDrivingMotorSpeed(int16_t speed1, int16_t speed2);

void Servo_DataRecvService(CAN_RxHeaderTypeDef* pRxHeader, uint8_t* pRxData);

void Servo_CanSendPage(uint8_t node,uint8_t *pSendBuff,uint8_t Num);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT ATOVO *****END OF FILE****/
