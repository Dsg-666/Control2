/**
  ******************************************************************************
  * @file     : mdw_ac_base.c
  * @author   : xiaofeng 
  * @version  : V1.0
  * @date     : 2019 07
  * @brief    : APP Atovo Comm
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MDW_ATOVO_COMM_BASE_H__
#define __MDW_ATOVO_COMM_BASE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "mdw_ac.h"
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint8_t UpdataTypeFlag;// 0:不升级 1：开始升级投递控制板 15：取消升级
	uint8_t	UpdataFileType;// 1:bin文件 2:tar.gz文件
	uint16_t	CMDID;
	uint32_t	FileSize;
}IAP_DT2DMB_CMD_t;

typedef struct
{
	uint32_t	FileOffset;
	uint16_t	CMDID;
	uint32_t	DataLen;
}IAP_DT2DMB_DATA_t;

#define DATALEN			(1024)
typedef struct
{
  uint8_t YmodemBuff[DATALEN+7];       //Ymodem帧数据：1字节的数据类型（保留）2字节的包计数器 2字节的包计数器的补码 128字节的数据内容 2字节的CRC16 以上共135字节
}DT2DMB_DATA_WRITE_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/	 

/* Exported functions --------------------------------------------------------*/
uint8_t AC_ControlRecFrameData(const ComDataType_t dataType);

void IAP_ACK_Pro(uint32_t offset,uint16_t CMD);
void IAP_REQ_Pro(uint32_t offset,uint16_t CMD,uint32_t length);

#ifdef __cplusplus
}
#endif

#endif 


/*****END OF FILE****/

