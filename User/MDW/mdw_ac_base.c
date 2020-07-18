/**
  ******************************************************************************
  * @file     : mdw_ac_base.c
  * @author   : xiaofeng 
  * @version  : V1.0
  * @date     : 2019 07
  * @brief    : APP Atovo Comm
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "mdw_ac_base.h"

#include "bsp_dio.h"
#include "app_update.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t JumpFlag;
extern uint32_t UpdataStates;
extern int16_t CanSendNum;
extern uint8_t RXDataFlag;



extern uint16_t ReceiveNum;
extern uint16_t ReceiveLen;
extern uint16_t ReceiveLenSUM;
extern uint16_t ReceiveStates;
extern uint32_t	currentOffset;
extern uint32_t	requiredFileSize;


uint32_t		currentCMDID=0x0;
uint32_t		currentRecvFileSize=0x0;

uint16_t		FileIndex=0;
uint8_t			cancelUpdate=0;

//flag area:
uint8_t		DATA_CMDID_WRONG = 0x0;
uint8_t		FILE_OFFSET_WRONG = 0x0;
uint8_t		FILE_SIZE_WRONG =0x0;
uint8_t		startRequest = 0x0;

uint8_t		FileRecord[4000];

IAP_DT2DMB_DATA_t	IAP_DT2DMB_DATA;
IAP_DT2DMB_CMD_t IAP_DT2DMB_CMD;
DT2DMB_DATA_WRITE_t DT2DMB_DATA_WRITE; 
/* Private function prototypes -----------------------------------------------*/
extern void STMFLASH_Erase(uint32_t startAddr, uint32_t endAddr);
/* Private functions ---------------------------------------------------------*/

uint8_t AC_ControlRecFrameData(const ComDataType_t dataType)
{
  switch (dataType)
  {
  case COMM_DT2PSB_UPDATE_DATA://DT2DMB 发送升级数据
    {
      RXDataFlag = 1;
      IAPFLAG_RX = 1;
			IAP_DT2DMB_DATA.FileOffset = m_tComRecvFrame.data[0] << 24 | m_tComRecvFrame.data[1] << 16 | m_tComRecvFrame.data[2] << 8 | m_tComRecvFrame.data[3];
			IAP_DT2DMB_DATA.CMDID = m_tComRecvFrame.data[4] << 8 | m_tComRecvFrame.data[5];
			IAP_DT2DMB_DATA.DataLen = m_tComRecvFrame.data[6] << 24 | m_tComRecvFrame.data[7] << 16 | m_tComRecvFrame.data[8] << 8 | m_tComRecvFrame.data[9];
			if(currentCMDID!=IAP_DT2DMB_DATA.CMDID){
					DATA_CMDID_WRONG = 1;
			}else{
				DATA_CMDID_WRONG=0;
			}
			if(currentOffset!=IAP_DT2DMB_DATA.FileOffset){
					FILE_OFFSET_WRONG = 1;
			}else{
				FILE_OFFSET_WRONG=0;
			}
			if(requiredFileSize!=IAP_DT2DMB_DATA.DataLen){
					FILE_SIZE_WRONG = 1;
			}else{
				FILE_SIZE_WRONG=0;
			}
			
      ReceiveLen = IAP_DT2DMB_DATA.DataLen;
      CanSendNum = ReceiveLen;
      memcpy((void *)&DT2DMB_DATA_WRITE.YmodemBuff, (void *)&(m_tComRecvFrame.data[10]), m_tComRecvFrame.dataLength);
    }
    break;
  case COMM_DT2PSB_UPDATE_CMD://DT2DMB 发送升级指令
    {// 0:不升级 8：开始升级上包架控制板 0F：取消升级
      RXDataFlag = 1;
			memset(&FileRecord,0,sizeof(FileRecord));
      IAP_DT2DMB_CMD.UpdataTypeFlag = m_tComRecvFrame.data[0];
			if(IAP_DT2DMB_CMD.UpdataTypeFlag == 0x0F){
				cancelUpdate=1;
				break;
			}
			if(IAP_DT2DMB_CMD.UpdataTypeFlag!=8)
				break;
			IAP_DT2DMB_CMD.UpdataFileType = m_tComRecvFrame.data[1];
			IAP_DT2DMB_CMD.CMDID = m_tComRecvFrame.data[2] << 8 | m_tComRecvFrame.data[3];
			currentCMDID = IAP_DT2DMB_CMD.CMDID;
			IAP_DT2DMB_CMD.FileSize = m_tComRecvFrame.data[4] << 24 | m_tComRecvFrame.data[5] << 16 | m_tComRecvFrame.data[6] << 8 | m_tComRecvFrame.data[7];
			FileIndex = IAP_DT2DMB_CMD.FileSize/DATALEN;
			if(IAP_DT2DMB_CMD.FileSize%DATALEN!=0)
				FileIndex+=1;
			currentRecvFileSize = IAP_DT2DMB_CMD.FileSize;
			startRequest=1;
    }
    break;
  default:
    break;
  }

  return 0x0;
}


void IAP_ACK_Pro(uint32_t offset,uint16_t CMD)
{
  quint16 count = 0x06;
  quint8 sendBuffer[16]=
  {
    0xBA, COMM_PSB2DT_UPDATE_DATA_ACK, 0x00, 0x0A,
    0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
  };
  sendBuffer[count++] = offset >> 24;
	sendBuffer[count++] = offset >> 16 & 0xFF;
	sendBuffer[count++] = offset >> 8 & 0xFF;
	sendBuffer[count++] = offset & 0xFF;
	
	sendBuffer[count++] = CMD >> 8;
	sendBuffer[count++] = CMD & 0xFF;


  qint16 CRCRet= AC_CalcCRC16(sendBuffer, count+4);
  //CRC
  sendBuffer[0x04] = CRCRet>>8;
  sendBuffer[0x05] = CRCRet;
		
	count = 16;

  App_Ethernet_ACSendData(sendBuffer, count);
}

void IAP_REQ_Pro(uint32_t offset,uint16_t CMD,uint32_t length)
{
  quint16 count = 0x06;
  quint8 sendBuffer[16]=
  {
    0xBA, COMM_PSB2DT_UPDATE_DATA_REQ, 0x00, 0x0A,
    0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
  };
	sendBuffer[count++] = offset >> 24;
	sendBuffer[count++] = offset >> 16 & 0xFF;
	sendBuffer[count++] = offset >> 8 & 0xFF;
	sendBuffer[count++] = offset & 0xFF;
	
	sendBuffer[count++] = CMD >> 8;
	sendBuffer[count++] = CMD & 0xFF;
	
	sendBuffer[count++] = length >> 24;
	sendBuffer[count++] = length >> 16 & 0xFF;
	sendBuffer[count++] = length >> 8 & 0xFF;
	sendBuffer[count++] = length & 0xFF;

  qint16 CRCRet= AC_CalcCRC16(sendBuffer, count);
  //CRC
  sendBuffer[0x04] = CRCRet>>8;
  sendBuffer[0x05] = CRCRet;

  App_Ethernet_ACSendData(sendBuffer, count);
}




/*********************************END OF FILE*********************/
