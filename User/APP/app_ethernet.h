/**
  ******************************************************************************
  * @file       : app_ethernet.h
  * @author     : xiaofeng 
  * @version    : V1.0
  * @date       : 201907
  * @brief      : STM32F4 Ethernet
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_ETHERNET_H__
#define __APP_ETHERNET_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported macro ------------------------------------------------------------*/	 
#define ETHERNET_COM_DATA_BUFFER_SIZE       (4096)

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint8_t sn;
  
  uint8_t destIP[4];
  uint16_t destPort;
  uint16_t localPort;
  
  uint8_t sendBuffer[ETHERNET_COM_DATA_BUFFER_SIZE];
  uint16_t sendCount;
  uint16_t sendBufferSize;
  
  uint8_t recvBuffer[ETHERNET_COM_DATA_BUFFER_SIZE];
  uint16_t recvCount;
  uint16_t recvBufferSize;
}EthernetComCtrl_t;

/* Exported constants --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
void App_EthernetInitConfig(void);

void App_EthernetServiceFunc(void);

uint8_t App_Ethernet_ACSendData(uint8_t *pSendBuffer, uint16_t uCount);

#ifdef __cplusplus
}
#endif

#endif 

/*****END OF FILE****/

