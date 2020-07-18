/**
  ******************************************************************************
  * @file     : app_update.c
  * @author   : xiaofeng 
  * @version  : V1.0
  * @date     : 2019 08
  * @brief    : APP IAP Update
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_UPDATE_H__
#define __APP_UPDATE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef struct 
{
  uint8_t flag;
}IAP_UpdateCtrl_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/	 
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define SPIFLASH_IAP_ADDR       (1024)      // 内存地址
#define SPIFLASH_IAP_SIZE       sizeof(bootloader)      // 占内存大小

#define BOOTLOADER_ADD          0x8000000    //升级程序地址
#define APP_START_ADD           ADDR_FLASH_SECTOR_5     //应用程序一地址 ：主运行程序
#define APP_END_ADD             ADDR_FLASH_SECTOR_9+GetSectorSize(ADDR_FLASH_SECTOR_9) - 0x01  
#define APP2_START_ADD          ADDR_FLASH_SECTOR_10    //应用程序二地址 ：校验用程序
#define APP2_END_ADD            ADDR_FLASH_SECTOR_11+GetSectorSize(ADDR_FLASH_SECTOR_11) - 0x01 

/* Exported functions --------------------------------------------------------*/
void App_UpdateServiceInit(void);
void App_UpdateServiceFunc(void);

uint32_t GetSectorSize(uint32_t Sector);

#ifdef __cplusplus
}
#endif

#endif 


/*****END OF FILE****/

