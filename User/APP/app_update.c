/**
  ******************************************************************************
  * @file     : app_update.c
  * @author   : xiaofeng 
  * @version  : V1.0
  * @date     : 2019 08
  * @brief    : APP IAP Update
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "app_update.h"
#include "cmsis_os.h"

#include "mdw_ac_base.h"

#include "bsp_spi_flash.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint32_t BootLoaderFlag;
  uint32_t BootLoaderType;
}BTF;


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern uint8_t gLEDFlashMode;
extern IAP_DT2DMB_CMD_t IAP_DT2DMB_CMD;
extern DT2DMB_DATA_WRITE_t DT2DMB_DATA_WRITE; 
extern uint32_t		currentRecvFileSize;
extern uint8_t		FileRecord[4000];
extern uint16_t   FileIndex;
extern uint8_t cancelUpdate;
extern uint8_t 	startRequest;
uint32_t recievedFileSize=0;
uint32_t currentOffset=0;
uint8_t JumpFlag = 0;
uint32_t UpdataStates = 0;
int16_t CanSendNum = 0x0;
uint16_t CanRTNum = 0;
uint8_t RXDataFlag = 0x0;

uint32_t IAPTimeOutCnt = 0;
uint8_t IAP_ACK_FLAG = 0x0;
uint8_t IAP_REQ_FLAG = 0x0;
uint8_t IAPFLAG_RX = 0x0;
uint8_t IAP_OVER_FLAG = 0;
uint8_t IAP_StartFlag = 0;

uint16_t ReceiveNum = 0;
uint16_t ReceiveLen = 0;
uint16_t ReceiveLenSUM = 0;
uint16_t ReceiveStates = 0;

BTF bootloader,bootloaderkeep;
uint32_t StartTimer = 0;
uint8_t StartRunFlag = 0;
uint8_t FLASH_UPDATA_FLAG = 0;
uint8_t IAP_DT2DMB_CMD_UpdataTypeFlagkeep = 0;

uint16_t PageNum = 0;
uint8_t PageReceiveFlag = 0;
uint16_t PageNumReceive = 0;
uint16_t PageNumkeep = 0;
uint32_t u32YmodemBuff[DATALEN/4] = {0};
uint8_t YmodemBuff[DATALEN+7] = {0};
uint8_t timeoutCancelCount=0;

/* Private function prototypes -----------------------------------------------*/

void App_ReadIAPConfigInfo( BTF *pDeviceInfo,uint32_t ADDR,uint16_t SIZE);
void App_WriteIAPConfigInfo( BTF *pDeviceInfo,uint32_t ADDR,uint16_t SIZE);

void jump_to_app(uint32_t app_address);
void STMFLASH_Erase(uint32_t startAddr, uint32_t endAddr);
void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);

static uint32_t GetSector(uint32_t Address);


/* Private functions ---------------------------------------------------------*/
void App_UpdateServiceInit(void)
{
  App_ReadIAPConfigInfo(&bootloader,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
  if((bootloader.BootLoaderFlag > 1)||(bootloader.BootLoaderType > 2))
  {
    bootloader.BootLoaderFlag = 0;
    bootloader.BootLoaderType = 1;
  }
  StartTimer = 0;
  RXDataFlag = 0;
  FLASH_UPDATA_FLAG = 0;
  STMFLASH_Erase(APP2_START_ADD, APP2_END_ADD); // 先擦除APP的Flash内容
}
/**
  * @brief  : IAP Update 应用服务函数
  * @note   : 
  * @param  : _arg - 
  * @retval : 无
  */
void App_UpdateServiceFunc(void)
{   
	uint8_t	AllCheck=0;
	uint16_t AllCheckCount=0;

    StartTimer++;
    if(StartTimer > 300)
    {
      StartRunFlag = 1;
      if(RXDataFlag == 1)
      {//收到任务升级指令时
        StartTimer = 300;
      }
      else
      {
        if(StartTimer > 60000)
        {//大于一分钟没有收到升级指令，则重启
          NVIC_SystemReset();
        }
      }  
    }  

    if(IAP_DT2DMB_CMD.UpdataTypeFlag != IAP_DT2DMB_CMD_UpdataTypeFlagkeep)
    {

      if(IAP_DT2DMB_CMD.UpdataTypeFlag == 1)
      {//运控板升级
          ;
      }
      else if(IAP_DT2DMB_CMD.UpdataTypeFlag == 15)
      {//取消运控升级
         if(IAP_OVER_FLAG != 2)
         {
          NVIC_SystemReset();
         }
      }
      IAP_DT2DMB_CMD_UpdataTypeFlagkeep = IAP_DT2DMB_CMD.UpdataTypeFlag;
    }

    if(StartRunFlag)
    {//正常启动后，启动判断
        if(((FLASH_UPDATA_FLAG == 0) && (bootloader.BootLoaderFlag == 0)) || cancelUpdate==1)
        {//跳转至主程序
						if(cancelUpdate==1){
							bootloader.BootLoaderFlag=0;
							bootloader.BootLoaderType=1;
							STMFLASH_Erase(APP2_START_ADD, APP2_END_ADD);
						}
            App_WriteIAPConfigInfo(&bootloader,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
            App_ReadIAPConfigInfo(&bootloaderkeep,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
            if((bootloaderkeep.BootLoaderFlag == bootloader.BootLoaderFlag) && (bootloader.BootLoaderFlag == 0))
            {
              jump_to_app(APP_START_ADD);
            }
            else
            {
              App_WriteIAPConfigInfo(&bootloader,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
            }
        }
        else
        {
            if(bootloader.BootLoaderType == 1)
            {//升级运动控制板        
                if(JumpFlag == 1)
                {
                    STMFLASH_Erase(APP_START_ADD, APP_END_ADD); // 先擦除APP的Flash内容
                    STMFLASH_Write(APP_START_ADD,(uint32_t *)APP2_START_ADD,recievedFileSize/4);
                    
                    bootloader.BootLoaderFlag = 0; 
                    //正确升级后自动跳转
                    App_WriteIAPConfigInfo(&bootloader,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
                    App_ReadIAPConfigInfo(&bootloaderkeep,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
                    if((bootloaderkeep.BootLoaderFlag == bootloader.BootLoaderFlag) && (bootloader.BootLoaderFlag == 0))
                    {
                      NVIC_SystemReset(); 
                      //jump_to_app(APP_START_ADD);
                    }
                    else
                    {
                      App_WriteIAPConfigInfo(&bootloader,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
                    }            
                }               
                if((IAP_StartFlag == 0)&&(bootloader.BootLoaderFlag == 1))
                {//保证升级伺服掉电后还可以直接进入运动控制板
                    RXDataFlag = 1;
                    FLASH_UPDATA_FLAG = 1;
                    bootloader.BootLoaderFlag = 0;
                    App_WriteIAPConfigInfo(&bootloader,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
                    App_ReadIAPConfigInfo(&bootloaderkeep,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
                    if(bootloaderkeep.BootLoaderFlag == bootloader.BootLoaderFlag)
                    {
                      IAP_StartFlag = 1;
                    }
                    else
                    {
                      App_WriteIAPConfigInfo(&bootloader,SPIFLASH_IAP_ADDR,SPIFLASH_IAP_SIZE);
                    }
                }
                switch(UpdataStates)
                {
                  case 0:
                  {//发送0x0C 开启传输
                    if(PageReceiveFlag == 1)
                    {//写入FLASH
											STMFLASH_Write(APP2_START_ADD + currentOffset, (uint32_t *)u32YmodemBuff ,(ReceiveLen/4));
											FileRecord[currentOffset/DATALEN]=1;
											currentOffset += ReceiveLen;
											recievedFileSize += ReceiveLen;
											PageReceiveFlag=0;
											for(int i=0;i<FileIndex;i++){
													if(FileRecord[i]==1)
														AllCheckCount++;
											}
											if(AllCheckCount==FileIndex)
												AllCheck=1;
											if (currentRecvFileSize==recievedFileSize && AllCheck==1){
												IAP_OVER_FLAG = 2;
											}
                      else
                      {
                        IAP_OVER_FLAG = 0;
                      }
                    }
                    else
                    {
											if(startRequest==1)
												IAP_REQ_FLAG = 1;   
                    }
                    break;
                  }
                  case 1:
                  {//等待上位回复开始传输的信号
                    if(IAPFLAG_RX == 1)
                    {//收到上位开始传输的数据 并处理
                        IAPFLAG_RX = 0;
                        IAPTimeOutCnt = 0;//清超时计数器
												IAP_ACK_FLAG = 1;
												memcpy((void *)&YmodemBuff, (void *)&(DT2DMB_DATA_WRITE.YmodemBuff), ReceiveLen);
												memcpy((uint8_t*)&u32YmodemBuff, (uint8_t*)YmodemBuff, ReceiveLen);
												PageReceiveFlag = 1;       
                      }
                      else
                      {//接收超时，重新传输
                        if((IAPTimeOutCnt++) > 1000)
                        {//10秒超时
                          UpdataStates = 0;
													timeoutCancelCount++;
                        }
												//if(timeoutCancelCount > 3)
													//cancelUpdate = 1;
                      }
                      if(IAP_OVER_FLAG >= 2)
                      {//传输正确结束 互相备份用
                        IAP_ACK_FLAG = 1; 
                        
                        break;
                      }
                    }
                    break;
                    default:
                    break;
                }
            }
            else
            {//等待上位指定需要升级哪块板子
              bootloader.BootLoaderType = 1;//升级MCB
            }
        }
    }
}

void App_ReadIAPConfigInfo( BTF *pDeviceInfo,uint32_t ADDR,uint16_t SIZE)
{
  W25Q128_ReadBuffer((uint8_t *)(pDeviceInfo), ADDR, SIZE);
}

void App_WriteIAPConfigInfo( BTF *pDeviceInfo,uint32_t ADDR,uint16_t SIZE)
{
  uint8_t uRet=0x0;
  do
  {
    uRet = W25Q128_WriteBuffer((uint8_t *)(pDeviceInfo), ADDR, SIZE);
  }while(uRet == 0x0);
}

void jump_to_app(uint32_t app_address)
{
    typedef void (*_func)(void);

    //__disable_irq();

    /* MCU peripherals re-initial. */
    {

        /* reset systick */
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

        /* disable all peripherals clock. */
        RCC->AHB1ENR = (1<<20); /* 20: F4 CCMDAT ARAMEN. */
        RCC->AHB2ENR = 0;
        RCC->AHB3ENR = 0;
        RCC->APB1ENR = 0;
        RCC->APB2ENR = 0;

        /* Switch to default cpu clock. */
        RCC->CFGR = 0;
    } /* MCU peripherals re-initial. */

    /* Disable MPU */
    MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;

    /* disable and clean up all interrupts. */
    {
        for(int i = 0; i < 8; i++)
        {
            /* disable interrupts. */
            NVIC->ICER[i] = 0xFFFFFFFF;

            /* clean up interrupts flags. */
            NVIC->ICPR[i] = 0xFFFFFFFF;
        }
    }

    /* Set new vector table pointer */
    SCB->VTOR = app_address;

    /* reset register values */
//    __set_BASEPRI(0);
//    __set_FAULTMASK(0);

    /* set up MSP and switch to it */
    __set_MSP(*(uint32_t*)app_address);
    __set_CONTROL(0);    
    __set_PSP(*(uint32_t*)app_address);


    /* ensure what we have done could take effect */
    __ISB();

    //__disable_irq();

    /* never return */
    ((_func)(*(uint32_t*)(app_address + 4)))();
}

void STMFLASH_Erase(uint32_t startAddr, uint32_t endAddr)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  uint32_t FirstSector = 0, NbOfSectors = 0;
  
  osDelay(1000);
  /* Unlock the Flash to enable the flash control register access *************/ 
  HAL_FLASH_Unlock();
  
    /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
  
  /* Get the 1st sector to erase */
  FirstSector = GetSector(startAddr);
  /* Get the number of sector to erase from 1st sector*/
  NbOfSectors = GetSector(endAddr) - FirstSector + 1;

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FirstSector;
  EraseInitStruct.NbSectors = NbOfSectors;
  
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  { 
    /* 
      Error occurred while sector erase. 
      User can add here some code to deal with this error. 
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
    */
    /*
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
    */
    //gLEDFlashMode = 0x02;
    Error_Handler();
  }

   /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
   you have to make sure that these data are rewritten before they are accessed during code
   execution. If this cannot be done safely, it is recommended to flush the caches by setting the
   DCRST and ICRST bits in the FLASH_CR register. */
  __HAL_FLASH_DATA_CACHE_DISABLE();
  __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

  __HAL_FLASH_DATA_CACHE_RESET();
  __HAL_FLASH_INSTRUCTION_CACHE_RESET();

  __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
  __HAL_FLASH_DATA_CACHE_ENABLE();
  
  /* Lock the Flash to disable the flash control register access (recommended
   to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock(); 
}

void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)
{ 
  uint32_t startAddr = WriteAddr;
  uint32_t endAddr = WriteAddr+NumToWrite*4; //写入结束地址
  
  /* Unlock the Flash to enable the flash control register access *************/ 
  HAL_FLASH_Unlock();
  
  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  while (startAddr < endAddr)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr, *pBuffer) == HAL_OK)
    {
      startAddr = startAddr + 4;
      pBuffer++;
    }
    else
    { 
      /* Error occurred while writing data in Flash memory. 
         User can add here some code to deal with this error */
      /*
        FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError();
      */
      //gLEDFlashMode = 0x02;
      Error_Handler();
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock(); 
}

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else
  {
    sector = FLASH_SECTOR_11;  
  }

  
  return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;

  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  
  return sectorsize;
}




/*******************************************END OF FILE***********************************/
