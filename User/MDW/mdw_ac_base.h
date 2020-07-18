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
  uint8_t UpdataTypeFlag;// 0:������ 1����ʼ����Ͷ�ݿ��ư� 15��ȡ������
	uint8_t	UpdataFileType;// 1:bin�ļ� 2:tar.gz�ļ�
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
  uint8_t YmodemBuff[DATALEN+7];       //Ymodem֡���ݣ�1�ֽڵ��������ͣ�������2�ֽڵİ������� 2�ֽڵİ��������Ĳ��� 128�ֽڵ��������� 2�ֽڵ�CRC16 ���Ϲ�135�ֽ�
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

