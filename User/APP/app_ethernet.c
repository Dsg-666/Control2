/**
  ******************************************************************************
  * @file       : app_ethernet.c
  * @author     : xiaofeng 
  * @version    : V1.0
  * @date       : 201907
  * @brief      : STM32F4 Ethernet
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "app_ethernet.h"

#include "w5500.h"
#include "socket.h"
#include "wizchip_conf.h"
#include <string.h>
//#include "bsp_dio.h"
#include "mdw_ac.h"
//#include "mdw_ac_base.h"

/* Private define ------------------------------------------------------------*/
#define APP_ETHERNET_LOCAL_TEST_MODE        (0x01)  // 0x0 - 关闭测试; 0x1 - UDP收发测试

/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if (APP_ETHERNET_LOCAL_TEST_MODE > 0x0)
#define SOCKET_DATA_BUF_SIZE            (1024*0x10)
uint8_t gDATABUF[SOCKET_DATA_BUF_SIZE];
#endif

extern uint8_t JumpFlag;
extern uint32_t UpdataStates;


extern uint16_t ReceiveNum;
extern uint16_t ReceiveLen;
extern uint16_t ReceiveLenSUM;
extern uint16_t ReceiveStates;

extern uint32_t currentCMDID;
extern uint32_t	currentOffset;

extern uint32_t currentRecvFileSize;
extern uint32_t recievedFileSize;

//flag area:
extern uint8_t		DATA_CMDID_WRONG;
extern uint8_t		FILE_OFFSET_WRONG;
extern uint8_t		FILE_SIZE_WRONG;

uint8_t	flagcount=0x00;

uint32_t	requiredFileSize=0x0;

static EthernetComCtrl_t sgACUdpCtrl;
/* Private functions ---------------------------------------------------------*/
#if (APP_ETHERNET_LOCAL_TEST_MODE > 0x0)
int32_t Ethernet_TestUDP(uint8_t* buf, uint16_t port);
static void App_EthernetLocalTest(void);
#endif
static int32_t Ethernet_UDPCom_RecvData(EthernetComCtrl_t *pUdpComCtrl);
static int32_t Ethernet_UDPCom_SendData(EthernetComCtrl_t *pUdpComCtrl);
static void Ethernet_UDPCom_AddSendData(EthernetComCtrl_t *pUdpComCtrl, uint8_t *sendBuffer, uint16_t count);
static int32_t Comm_Ethernet_UDP(EthernetComCtrl_t *pUdpComCtrl);

/**
  * @brief  : Ethernet 应用服务函数
  * @note   : 无
  * @param  : _arg - 
  * @retval : 无
  */
void App_EthernetServiceFunc(void)
{
  App_EthernetLocalTest();

}
/**
  * @brief  : Ethernet AC 发送数据
  * @note   : 无
  * @param  : sendBuffer - 
  * @retval : 无
  */
uint8_t App_Ethernet_ACSendData(uint8_t *pSendBuffer, uint16_t uCount)
{
  Ethernet_UDPCom_AddSendData(&sgACUdpCtrl, pSendBuffer, uCount);
  
  return 0x0;
}

/*
 * 函数名：Comm_Ethernet_UDP
 * 描述  ：网口 UDP 通信
 * 输入  ：sn - Socket 序号 (0-7)
           pUdpComCtrl
 * 输出  : 无
 * 调用  ：内部调用
 */

static int32_t Comm_Ethernet_UDP(EthernetComCtrl_t *pUdpComCtrl)
{
  int32_t  ret = 0x0;
  uint16_t recvSize = 0x0;
  uint16_t sentSize = 0x0;

  switch(getSn_SR(pUdpComCtrl->sn))
  {
    case SOCK_UDP :
      if((recvSize = getSn_RX_RSR(pUdpComCtrl->sn)) > 0) { // UDP Recv 
        if(recvSize > pUdpComCtrl->recvBufferSize) 
        {// 判断接收缓冲区数据量
          recvSize = pUdpComCtrl->recvBufferSize;
        }
        ret = recvfrom(pUdpComCtrl->sn, pUdpComCtrl->recvBuffer, recvSize, pUdpComCtrl->destIP, &pUdpComCtrl->destPort);
        if(ret <= 0) {
           return ret;
        }
        
        pUdpComCtrl->recvCount = recvSize;
      }
      else {
        pUdpComCtrl->recvCount = 0x0;
      }
      
      
      if( pUdpComCtrl->sendCount > 0x0 ) { // UDP Send
        sentSize = 0;
        while(sentSize != pUdpComCtrl->sendCount) {
          ret = sendto(pUdpComCtrl->sn, pUdpComCtrl->sendBuffer+sentSize, pUdpComCtrl->sendCount-sentSize, pUdpComCtrl->destIP, pUdpComCtrl->destPort);
          if(ret < 0) {
            return ret;
          }
          sentSize += ret; // Don't care SOCKERR_BUSY, because it is zero.
        }
        
        pUdpComCtrl->sendCount = 0x0;
      }
      break;
    case SOCK_CLOSED:
      {
        if((ret = socket(pUdpComCtrl->sn, Sn_MR_UDP, pUdpComCtrl->localPort, 0x00)) != pUdpComCtrl->sn)
        return ret;
      }
      break;
    default :
    break;
  }
  
  return 1;
}

/**
  * @brief  : Ethernet 应用初始化配置
  * @note   : 无
  * @param  : _arg - 
  * @retval : 无
  */
void App_EthernetInitConfig(void)
{
  uint8_t sgACServerIP[4] = {192, 168, 1, 11};
  memcpy(sgACUdpCtrl.destIP, sgACServerIP, 0x04);
  sgACUdpCtrl.sn = 0x01; 
  sgACUdpCtrl.destPort = 8066;
  sgACUdpCtrl.localPort = 8066;
  sgACUdpCtrl.recvCount = 0x0;
  sgACUdpCtrl.sendCount = 0x0; 
  sgACUdpCtrl.sendBufferSize = ETHERNET_COM_DATA_BUFFER_SIZE;
  sgACUdpCtrl.recvBufferSize = ETHERNET_COM_DATA_BUFFER_SIZE;
}

/**
  * @brief  : Ethernet 通信 UDP 添加发送
  * @note   : 无
  * @param  : sendBuffer - 发送数据缓冲区
              count - 发送数据长度
  * @retval : 无
  */
static void Ethernet_UDPCom_AddSendData(EthernetComCtrl_t *pUdpComCtrl, uint8_t *sendBuffer, uint16_t count)
{
  if(pUdpComCtrl->sendCount + count > pUdpComCtrl->sendBufferSize)
  {
    sgACUdpCtrl.sendCount = 0x0;
  }
  
  memcpy( (void *)(pUdpComCtrl->sendBuffer + pUdpComCtrl->sendCount), sendBuffer, count);
  pUdpComCtrl->sendCount += count;
}

/**
  * @brief  : Ethernet 通信 UDP 接收 
  * @note   : 无
  * @param  : _arg - 
  * @retval : 无
  */
static int32_t Ethernet_UDPCom_RecvData(EthernetComCtrl_t *pUdpComCtrl)
{
  int32_t  ret = 0x0;
  uint16_t recvSize = 0x0;

  switch(getSn_SR(pUdpComCtrl->sn))
  {
    case SOCK_UDP : // UDP Recv 
      if((recvSize = getSn_RX_RSR(pUdpComCtrl->sn)) > 0)
      {
        if(recvSize > pUdpComCtrl->recvBufferSize) 
        {// 判断接收缓冲区数据量
          recvSize = pUdpComCtrl->recvBufferSize;
        }

        ret = recvfrom(pUdpComCtrl->sn, pUdpComCtrl->recvBuffer, recvSize, pUdpComCtrl->destIP, &pUdpComCtrl->destPort);
        if(ret <= 0)
        {
           return ret;
        }
        
        pUdpComCtrl->recvCount = recvSize;
      }
      else
      {
        pUdpComCtrl->recvCount = 0x0;
      }
      break;
    case SOCK_CLOSED:
      {
        if((ret = socket(pUdpComCtrl->sn, Sn_MR_UDP, pUdpComCtrl->localPort, 0x00)) != pUdpComCtrl->sn)
        return ret;
      }
      break;
    default :
    break;
  }
  
  return 0x0;
}

/**
  * @brief  : Ethernet 通信 UDP 发送
  * @note   : 无
  * @param  : _arg - 
  * @retval : 无
  */
static int32_t Ethernet_UDPCom_SendData(EthernetComCtrl_t *pUdpComCtrl)
{
  int32_t  ret = 0x0;
  uint16_t sentSize = 0x0;
  
  if( pUdpComCtrl->sendCount == 0x0 )   
  {
    return 0x0;
  }
  
  switch(getSn_SR(pUdpComCtrl->sn))
  {
  case SOCK_UDP : // UDP Send
    sentSize = 0;
    while(sentSize != pUdpComCtrl->sendCount)
    {
      ret = sendto(pUdpComCtrl->sn, pUdpComCtrl->sendBuffer+sentSize, pUdpComCtrl->sendCount-sentSize, pUdpComCtrl->destIP, pUdpComCtrl->destPort);
      if(ret < 0)
      {
        return ret;
      }
      sentSize += ret; // Don't care SOCKERR_BUSY, because it is zero.
    }
    pUdpComCtrl->sendCount = 0x0;
    break;
  case SOCK_CLOSED:
    if((ret = socket(pUdpComCtrl->sn, Sn_MR_UDP, pUdpComCtrl->localPort, 0x00)) != pUdpComCtrl->sn)
    return ret;
  default :
    break;
  }
  
  return 0x0;
}

#if (APP_ETHERNET_LOCAL_TEST_MODE > 0x0)
/**
  * @brief  : LED 应用本地功能测试
  * @note   : 无
  * @param  : 无 
  * @retval : 无
  */
static void App_EthernetLocalTest(void)
{
#if (APP_ETHERNET_LOCAL_TEST_MODE == 0x01)
  Ethernet_TestUDP(gDATABUF, 8080);
#endif
}


/**
  * @brief  : Ethernet 应用服务函数
  * @note   : 无
  * @param  : _arg - 
  * @retval : 无
  */
int32_t Ethernet_TestUDP(uint8_t* buf, uint16_t port)
{
  uint8_t sn = 0x0;   //Socket 0
  int32_t  ret;
  uint16_t size, sentsize;
  uint8_t  destip[4];
  uint16_t destport;

  switch(getSn_SR(sn))
  {
    case SOCK_UDP :
       if((size = getSn_RX_RSR(sn)) > 0)
       {
          if(size > SOCKET_DATA_BUF_SIZE) size = SOCKET_DATA_BUF_SIZE;
          ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
          if(ret <= 0)
          {
             return ret;
          }
          size = (uint16_t) ret;
          sentsize = 0;
          while(sentsize != size)
          {
             ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
             if(ret < 0)
             {
                return ret;
             }
             sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
          }
       }
       break;
    case SOCK_CLOSED:
       if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
          return ret;
       break;
    default :
       break;
  }
  return 1;
}
#endif

/*****END OF FILE****/
