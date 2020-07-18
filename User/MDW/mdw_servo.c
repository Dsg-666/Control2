/**
  ******************************************************************************
  * @file           : mdw_servo.h
  * @brief          : Servo Driving 
  ******************************************************************************
  */
/* Private includes ----------------------------------------------------------*/
#include "mdw_servo.h"

#include "can.h"
#include "bsp_can2.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
// Servo 通信 帧头
typedef struct 
{
  uint32_t Des_ID:8;
  uint32_t Mode:1;
  uint32_t Priority:2;
  uint32_t Reserved:21;
}__attribute__((aligned(1),packed)) Servo_FrameIDRegion_t;

// Servo CAN 通信 数据组
typedef struct 
{
  uint8_t  Src_ID; 
  uint8_t  Fun_Num;
  uint16_t Addr;
  int16_t Data1;
  int16_t Data2;
}__attribute__((aligned(1),packed))  Servo_FrameDataRegion_t;

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
Servo_CtrlData_t gLeftServoData =  {
  .gearRatio = 26,
};
Servo_CtrlData_t gRightServoData = {
  .gearRatio = 26,
};

#if (SERVO_SUPPORT_TOP_LIFT_MOTOR > 0x0)
Servo_CtrlData_t gTopServoData = {
  .gearRatio = 26,
};
Servo_CtrlData_t gLiftServoData = {
  .gearRatio = 26,
};
#endif  // SERVO_SUPPORT_TOP_LIFT_MOTOR

#if (SERVO_SUPPORT_CAN_SEND_MUTEX > 0x0)
extern osMutexId_t ServoCtrlMutexHandle;
#endif // SERVO_SUPPORT_CAN_SEND_MUTEX

/* Private function prototypes -----------------------------------------------*/
static uint8_t Servo_SendDataFrame( uint8_t idPriority, uint8_t idMode, uint8_t idDes, uint8_t idSrc, 
                                    uint8_t funcCode, uint16_t funcAddr, uint8_t dlc, int16_t Data1, int16_t Data2);
static void Servo_StartUpInit(uint8_t _Node);
/* Private user code ---------------------------------------------------------*/

/**
  * @brief  Servo 初始化
  * @note   使用 Servo Ctrl 必须调用这个函数，否则会导致操作结果异常
  * @param  None
  * @retval None
  */
void Servo_InitConfig(void)
{
  Servo_StartUpInit(SERVO_LEFT_NODE_ID);
  Servo_StartUpInit(SERVO_RIGHT_NODE_ID);
  
#if (SERVO_SUPPORT_TOP_LIFT_MOTOR > 0x0)
  Servo_StartUpInit(SERVO_TOP_NODE_ID);
  Servo_StartUpInit(SERVO_LIFT_NODE_ID);
#endif
}

/**
  * @brief  Servo 数据接收处理服务
  * @note   
  * @param  None
  * @retval None
  */
void Servo_DataRecvService(CAN_RxHeaderTypeDef* pRxHeader, uint8_t* pRxData) 
{
  uint16_t Low  = 0x0;
  uint16_t High = 0x0;

  if((0x00 == pRxData[0])&& (0xCA == pRxData[1]))
  {
    if(0x0601 == pRxHeader->StdId) //left driver counter
    {
      Low  = (pRxData[2]<<8) + (pRxData[3]);
      High = (pRxData[4]<<8) + (pRxData[5]);
      gLeftServoData.encValue = (((High << 16) + Low) / gLeftServoData.gearRatio);
    }
    else if(0x0602 == pRxHeader->StdId) //right driver counter
    {
      Low  = (pRxData[2]<<8) + (pRxData[3]);
      High = (pRxData[4]<<8) + (pRxData[5]);
      gRightServoData.encValue = (((High << 16) + Low) / gRightServoData.gearRatio);
    }
#if (SERVO_SUPPORT_TOP_LIFT_MOTOR > 0x0)
    else if(0x0604 == pRxHeader->StdId) //反转 driver counter
    {
      Low  = (pRxData[2]<<8) + (pRxData[3]);
      High = (pRxData[4]<<8) + (pRxData[5]);
      gTopServoData.encValue = (((High << 16) + Low) / gTopServoData.gearRatio);
      gTopServoData.encValue += gTopServoData.encOffsetValue;
    }
    else if(0x0605 == pRxHeader->StdId) //提升 driver counter
    {
      Low  = (pRxData[2]<<8) + (pRxData[3]);
      High = (pRxData[4]<<8) + (pRxData[5]);
      gLiftServoData.encValue = (((High << 16) + Low)/ gLiftServoData.gearRatio);
      gLiftServoData.encValue += gLiftServoData.encOffsetValue;
    }
#endif // SERVO_SUPPORT_TOP_LIFT_MOTOR
  }
  else if((0x00 == pRxData[0])&& (0xD2 == pRxData[1]))
  {
    if(0x0601 == pRxHeader->StdId) //left driver counter
    {
      Low  = (pRxData[4]<<8) + (pRxData[5]);
      High = (pRxData[6]<<8) + (pRxData[7]);
      gLeftServoData.stateCode = (((High << 16) + Low));
    }
    else if(0x0602 == pRxHeader->StdId) //right driver counter
    {
      Low  = (pRxData[4]<<8) + (pRxData[5]);
      High = (pRxData[6]<<8) + (pRxData[7]);
      gRightServoData.stateCode = (((High << 16) + Low));
    }
#if (SERVO_SUPPORT_TOP_LIFT_MOTOR > 0x0)
    else if(0x0604 == pRxHeader->StdId) //left driver counter
    {
      Low  = (pRxData[4]<<8) + (pRxData[5]);
      High = (pRxData[6]<<8) + (pRxData[7]);
      gTopServoData.stateCode = (((High << 16) + Low));
      
    }
    else if(0x0605 == pRxHeader->StdId) //right driver counter
    {
      Low  = (pRxData[4]<<8) + (pRxData[5]);
      High = (pRxData[6]<<8) + (pRxData[7]);
      gLiftServoData.stateCode = (((High << 16) + Low));
    }
#endif // SERVO_SUPPORT_TOP_LIFT_MOTOR
  }
  
}

/**
  * @brief  设置单个电机速度
  * @note   
  * @param  node - 
            speed - 
  * @retval None
  */
uint8_t Servo_SetMotorSpeed(uint8_t node, int16_t speed)
{
  uint16_t speed_tmp = 0x0;

  if (0 <= speed)
  {
    speed   = (speed << 8) | (speed >> 8);
  }
  else 
  {
    speed_tmp = (~(~speed + 1) + 1);
    speed = (speed_tmp << 8) | (speed_tmp >> 8);
  }

  return ( Servo_SendDataFrame(0x03, 0x01, node, 0x00, 0x28, 0x0, 0x06, speed, 0x0) );
}

/**
  * @brief  设置两个电机速度
  * @note   
  * @param  node - 
            speed1 - 
            speed2 - 
  * @retval None
  */
uint8_t Servo_SetDrivingMotorSpeed(int16_t speed1, int16_t speed2)
{
  uint16_t speed_tmp;

  if (0 <= speed1)
  {
    speed1 = (speed1 << 8) | (speed1 >> 8);
  }
  else 
  {
    speed_tmp = (~(~speed1 + 1) + 1);
    speed1 = (speed_tmp << 8) | (speed_tmp >> 8);    
  }
  
  if (0 <= speed2)
  {
    speed2= (speed2 << 8) | (speed2 >> 8);
  }
  else 
  {
    speed_tmp = (~(~speed2 + 1) + 1);
    speed2 = (speed_tmp << 8) | (speed_tmp >> 8);    
  }

  return ( Servo_SendDataFrame(0x03, 0x01, 0x01, 0x00, 0x28, 0x01, 0x08, speed1, speed2) );
}
/**
  * @brief  设置电机使能
  * @note   
  * @param  node - 
            speed1 - 
  * @retval None
  */
void Servo_MotorEnable(uint8_t node)
{
  uint16_t Addr = (0x10 << 8) | (0x10 >> 8);
  int16_t Data1 = (0x01 << 8) | (0x01 >> 8);

  Servo_SendDataFrame(0x03, 0x01, node, 0x00, 0x24, Addr, 0x06, Data1, 0x0);
}

/**
  * @brief  设置电机失能
  * @note   
  * @param  node - 
            speed1 - 
  * @retval None
  */
void Servo_MotorDisable(uint8_t node)
{
  uint16_t Addr = (0x10 << 8) | (0x10 >> 8);
  int16_t Data1 = 0x0;

  Servo_SendDataFrame(0x03, 0x01, node, 0x00, 0x24, Addr, 0x06, Data1, 0x0);
}

/**
  * @brief  设置电机抱闸指令
  * @note   
  * @param  node - 
            cmd - 3:打开  ; 2:抱死
  * @retval None
  */
void Servo_MotorBreakStop(uint8_t node,uint8_t cmd)
{
  uint16_t Addr = 0x01C0;
  int16_t Data1 = (cmd << 8) | (cmd >> 8);;

  Servo_SendDataFrame(0x03, 0x01, node, 0x00, 0x24, Addr, 0x06, Data1, 0x0);
}

/**
  * @brief  设置电机急停指令
  * @note   
  * @param  node - 
            cmd - 4:正常运行  ; 5:电机急停
  * @retval None
  */
void Servo_MotorEmergencyStop(uint8_t node,uint8_t cmd)
{
  uint16_t Addr = 0x01C0;
  int16_t Data1 = (cmd << 8) | (cmd >> 8);;

  Servo_SendDataFrame(0x03, 0x01, node, 0x00, 0x24, Addr, 0x06, Data1, 0x0);
}

/**
  * @brief  设置清空多圈编码器数值
  * @note   
  * @param  node - 
  * @retval None
  */
void Servo_ClearEncoderCount(uint8_t node)
{
  uint16_t Addr = (0x04 << 8) | (0x04 >> 8);

  Servo_SendDataFrame(0x03, 0x01, node, 0x00, 0x2B, Addr, 0x08, 0x0, 0x0);
}

/**
  * @brief  设置上报内容
  * @note   
  * @param  node - 
  * @retval None
  */
void Servo_SetAutoReportContent(uint8_t node, int16_t Content1, int16_t Content2)
{
  uint16_t tmpValue = 0x0;
  uint16_t Addr = (0xEA << 8) | (0xEA >> 8);
  
  if (0 <= Content1)
  {
    Content1 = (Content1 << 8) | (Content1 >> 8);
  }
  else 
  {
    tmpValue = (~(~Content1 + 1) + 1);
    Content1 = (tmpValue << 8) | (tmpValue >> 8);
  }

  if (0 <= Content2)
  {
    Content2 = (Content2 << 8) | (Content2 >> 8);
  }
  else 
  {
    tmpValue = (~(~Content2 + 1) + 1);
    Content2 = (tmpValue << 8) | (tmpValue >> 8);
  }
  
  Servo_SendDataFrame(0x03, 0x01, node, 0x00, 0x21, Addr, 0x08, Content1, Content2);
}

void Servo_CanSendPage(uint8_t node,uint8_t *pSendBuff,uint8_t Num)
{
  uint32_t canSendStdId = 0x0;
  uint8_t canSendData[8] = {0x0};
  Servo_FrameIDRegion_t    Id;

  Id.Priority = 3;
  Id.Mode     = 1;
  Id.Des_ID   = node;
  memcpy(&canSendStdId, &Id, sizeof (Servo_FrameIDRegion_t));

  memcpy(canSendData, pSendBuff, Num);
   
  CAN2_SendStdData(canSendStdId, canSendData, Num);
}

 /**
  * @brief  伺服发送帧数据
  * @note   
  * @param  idPriority - 
            idMode - 
  * @retval None
  */
static uint8_t Servo_SendDataFrame( uint8_t idPriority, uint8_t idMode, uint8_t idDes, uint8_t idSrc, 
                                    uint8_t funcCode, uint16_t funcAddr, uint8_t dlc, int16_t Data1, int16_t Data2)
{
  uint8_t ret = 0x0;

  uint32_t canSendStdId = 0x0;
  uint8_t canSendData[8] = {0x0};
  
  Servo_FrameIDRegion_t    Id;
  Servo_FrameDataRegion_t  Data;

  // 通信帧头 组帧
  Id.Priority = idPriority;
  Id.Mode     = idMode;
  Id.Des_ID   = idDes;
  memcpy(&canSendStdId, &Id, sizeof (Servo_FrameIDRegion_t));

  // 通信帧数据 组帧
  Data.Src_ID  = idSrc;
  Data.Fun_Num = funcCode;
  Data.Addr    = funcAddr;
  Data.Data1   = Data1;    
  Data.Data2   = Data2;
  memcpy(canSendData, &Data, dlc);

#if (SERVO_SUPPORT_CAN_SEND_MUTEX > 0x0)
  if( osOK != osMutexAcquire(ServoCtrlMutexHandle, osWaitForever) ) { return 0xFE; }
  ret = CAN2_SendStdData(canSendStdId, canSendData, dlc);
  if( osOK != osMutexRelease(ServoCtrlMutexHandle) ) { return 0xFE; }
#else
  ret = CAN2_SendStdData(canSendStdId, canSendData, dlc);
#endif  // SERVO_SUPPORT_CAN_SEND_MUTEX
  
  return (ret);
}

 /**
  * @brief  伺服启动初始化
  * @note   
  * @param  idPriority - 
            idMode - 
  * @retval None
  */
static void Servo_StartUpInit(uint8_t _Node)
{
  Servo_MotorBreakStop(_Node, 0x03);      // 抱闸打开，电机可以自由运行
  osDelay(2);
  Servo_MotorEmergencyStop(_Node, 0x04);  // 电机非急停（正常运行）
  osDelay(2);
  Servo_MotorBreakStop(_Node, 0x08);      // 开启状态上报模式
  osDelay(2);
  Servo_ClearEncoderCount(_Node);         // 清空编码器计数
  osDelay(2);
  Servo_MotorEnable(_Node);               // 使能伺服
  osDelay(2);
  Servo_SetAutoReportContent(_Node, 0x001D, 0x001E); //开启自动上报 编码器值和当前速度值
  osDelay(2);
}

/********END OF FILE****/
