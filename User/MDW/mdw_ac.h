/**
  ******************************************************************************
  * @file     : mdw_ac.c
  * @author   : xiaofeng 
  * @version  : V1.0
  * @date     : 2019 07
  * @brief    : APP Atovo Comm
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MDW_ATOVO_COMM_H__
#define __MDW_ATOVO_COMM_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "app_ethernet.h"

/* Exported types ------------------------------------------------------------*/
typedef unsigned char   quint8;
typedef unsigned short  quint16;
typedef signed short    qint16;
typedef signed int      qint32;

// 通讯数据类型
typedef enum _AC_Com_DataType_
{
  //基础
  CDT_None,
	COMM_DT2PSB_UPDATE_CMD=0xB1,
	COMM_PSB2DT_UPDATE_DATA_REQ,
	COMM_DT2PSB_UPDATE_DATA,
	COMM_PSB2DT_UPDATE_DATA_ACK,

    CDT_END,                    // 数据类型结束
}ComDataType_t;

typedef struct
{  
  ComDataType_t dataType;                     // 数据类型
  quint16 dataLength;                         // 数据段长度
  quint8 data[ETHERNET_COM_DATA_BUFFER_SIZE]; // 数据缓冲区
}ComFrameData_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/	 
extern ComFrameData_t m_tComRecvFrame;

/* Exported functions --------------------------------------------------------*/


uint8_t AC_ProcessFrameData(EthernetComCtrl_t *pComCtrl);

quint16 AC_CalcCRC16(quint8 *buffer, quint16 length);

#ifdef __cplusplus
}
#endif

#endif 


/*****END OF FILE****/

