/**
  ******************************************************************************
  * @file           : bsp_w5500.h
  * @brief          : W5500 网络芯片 BSP
  ******************************************************************************
  *           硬件连接
  *           PA2 -> W5500_INT
  *           PA3 -> W5500_RST       
  *           PA4 -> W5500_SCS      
  *           PA5 -> W5500_SCK    
  *           PA6 -> W5500_MISO    
  *           PA7 -> W5500_MOSI  
  */
/* Private includes ----------------------------------------------------------*/
#include "bsp_w5500.h"

#include "main.h"
#include "spi.h"

#include <stdio.h>
#include <stdlib.h>

#include "w5500.h"
#include "wizchip_conf.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define W5500_TIMEOUT_MAX       (50)

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void W5500_Delay(uint32_t _Count);

static void W5500_Network_Init(void);

static void W5500_CRIS_Enter(void);
static void W5500_CRIS_Exit(void);
static void W5500_CS_Select(void);
static void W5500_CS_Deselect(void);
static uint8_t W5500_ReadByte(void);
static void W5500_WriteByte(uint8_t byte);
static void W5500_ReadBurst(uint8_t* pBuf, uint16_t len);
static void W5500_WriteBurst(uint8_t* pBuf, uint16_t len);
/* Private user code ---------------------------------------------------------*/

/**
  * @brief  : W5500_Init
  * @note   : 初始化
  * @param  :
  * @retval :
  */
void W5500_Init(void)
{
  W5500_Delay(1000);// 短暂延时,不然导致初始化失败

  reg_wizchip_cris_cbfunc(W5500_CRIS_Enter, W5500_CRIS_Exit); 
  reg_wizchip_cs_cbfunc(W5500_CS_Select, W5500_CS_Deselect);
  reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte); 
  reg_wizchip_spiburst_cbfunc(W5500_ReadBurst, W5500_WriteBurst);
  
  W5500_Network_Init();
}

/**
  * @brief  : W5500_Network_Init
  * @note   : W5500 网络初始化
  * @param  :
  * @retval :
  */
wiz_NetInfo gGetWizNetInfo;
wiz_NetTimeout gGetWizNetTimeout;

static void W5500_Network_Init(void)
{
  wiz_NetInfo setWizNetInfo ={.mac = {0x10, 0x1F, 0xDE, 0x12, 0x42, 0x30},
                              .ip = {192, 168, 1, 31},
                              .sn = {255,255,255,0},
                              .gw = {192, 168, 1, 1},
                              .dns = {8,8,8,8},
                              .dhcp = NETINFO_STATIC };

  ctlnetwork(CN_SET_NETINFO, (void*)&setWizNetInfo);
  ctlnetwork(CN_GET_NETINFO, (void*)&gGetWizNetInfo);

  wiz_NetTimeout setWizNetTimeout = { .retry_cnt = 10,    
                                      .time_100us = 100 };
  ctlnetwork(CN_SET_TIMEOUT, (void*)&setWizNetTimeout);
  ctlnetwork(CN_GET_TIMEOUT, (void*)&gGetWizNetTimeout);
             
  ctlwizchip(CW_INIT_WIZCHIP, NULL);     
									  
#if 1
    // Display Network Information
  uint8_t tmpstr[6];
  ctlwizchip(CW_GET_ID,(void*)tmpstr);
  printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
  printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gGetWizNetInfo.mac[0],gGetWizNetInfo.mac[1],gGetWizNetInfo.mac[2],
      gGetWizNetInfo.mac[3],gGetWizNetInfo.mac[4],gGetWizNetInfo.mac[5]);
  printf("SIP: %d.%d.%d.%d\r\n", gGetWizNetInfo.ip[0],gGetWizNetInfo.ip[1],gGetWizNetInfo.ip[2],gGetWizNetInfo.ip[3]);
  printf("GAR: %d.%d.%d.%d\r\n", gGetWizNetInfo.gw[0],gGetWizNetInfo.gw[1],gGetWizNetInfo.gw[2],gGetWizNetInfo.gw[3]);
  printf("SUB: %d.%d.%d.%d\r\n", gGetWizNetInfo.sn[0],gGetWizNetInfo.sn[1],gGetWizNetInfo.sn[2],gGetWizNetInfo.sn[3]);
  printf("DNS: %d.%d.%d.%d\r\n", gGetWizNetInfo.dns[0],gGetWizNetInfo.dns[1],gGetWizNetInfo.dns[2],gGetWizNetInfo.dns[3]);
  printf("======================\r\n");
#endif
									  
}

/**
  * @brief	: 临界段进入
  * @note	  : 
  * @param 	:
  * @retval	:
  */
static void W5500_CRIS_Enter(void)
{
}

/**
  * @brief	: 临界段退出
  * @note	  : 
  * @param 	:
  * @retval	:
  */
static void W5500_CRIS_Exit(void)
{
}

/**
  * @brief	: WIZ_CS
  * @note	  : 选择W5500, 置W5500的SCSn为低电平
  * @param 	:
  * @retval	:
  */
static void W5500_CS_Select(void)
{
  HAL_GPIO_WritePin(W5500_NSS_GPIO_Port, W5500_NSS_Pin, GPIO_PIN_RESET);
}

/**
  * @brief	: SPI_CS_Deselect
  * @note	  : 去选择W5500, 置W5500的SCS为高电平
  * @param 	:
  * @retval	:
  */
static void W5500_CS_Deselect(void)
{
  HAL_GPIO_WritePin(W5500_NSS_GPIO_Port, W5500_NSS_Pin, GPIO_PIN_SET);
}

/**
  * @brief	: W5500_WriteByte
  * @note	  : function to write byte using SPI
  * @param 	:
  * @retval	:
  */
static void W5500_WriteByte(uint8_t byte)
{
  HAL_SPI_Transmit(&hspi1,&byte,1,W5500_TIMEOUT_MAX);
}

/**
  * @brief	: W5500_ReadByte
  * @note	  : function to read byte using SPI
  * @param 	:
  * @retval	:
  */
static uint8_t W5500_ReadByte(void)
{
  uint8_t RxData=0x0;

  HAL_SPI_Receive(&hspi1,&RxData, 1, W5500_TIMEOUT_MAX);  
  return (RxData);
}

/**
  * @brief	: W5500_ReadBytes
  * @note	  : Wfunction to burst read using SPI
  * @param 	:
  * @retval	:
  */
static void W5500_ReadBurst(uint8_t* pBuf, uint16_t len)
{
  HAL_SPI_Receive(&hspi1, pBuf, len, W5500_TIMEOUT_MAX); 
}

/**
  * @brief	: W5500_ReadBytes
  * @note	  : function to burst write using SPI
  * @param 	:
  * @retval	:
  */
static void W5500_WriteBurst(uint8_t* pBuf, uint16_t len)
{
  HAL_SPI_Transmit(&hspi1, pBuf, len, W5500_TIMEOUT_MAX); 
}

/**
  * @brief	: W5500_HardReset
  * @note	  : 该引脚需要保持低电平至少 500 us，才能重置 W5500；
  * @param 	:
  * @retval	:
  */
static void W5500_Delay(uint32_t _Count)
{
  HAL_Delay(_Count);
}
/********END OF FILE****/
