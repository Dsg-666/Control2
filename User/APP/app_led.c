/**
  ******************************************************************************
  * @file     : app_led.c
  * @author   : xiaofeng 
  * @version  : V1.0
  * @date     : 2019 07
  * @brief    : APP LED 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "app_led.h"

#include "bsp_dio.h"

/* Private define ------------------------------------------------------------*/
#define APP_LED_LOCAL_TEST_MODE       (0x00)  // 0x0 - �رղ���; 0x1 - ����LED����; 0x2 - ����LED����; 0x3 - ���ط���������

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint32_t gLEDTaskRunCount;

uint8_t gLEDFlashMode = 0x0;
uint8_t gLEDFlashModePre = 0x0;

/* Private function prototypes -----------------------------------------------*/
#if (APP_LED_LOCAL_TEST_MODE > 0x0)
static void App_LEDLocalTest(void);
#endif
/* Private functions ---------------------------------------------------------*/

void App_LEDServiceInit(void)
{
  DIO_Ctrl(IO_AGV_LED_R, DIO_CTRL_OFF);
  DIO_Ctrl(IO_AGV_LED_G, DIO_CTRL_OFF); 
  DIO_Ctrl(IO_AGV_LED_B, DIO_CTRL_OFF);  
}
/**
  * @brief  : LED Ӧ�÷�����
  * @note   : 
  * @param  : _arg - 
  * @retval : ��
  */
void App_LEDServiceFunc(void *_arg)
{
#if (APP_LED_LOCAL_TEST_MODE == 0x0)
    if( gLEDFlashModePre != gLEDFlashMode )
    {
      App_LEDServiceInit();
      if(gLEDFlashMode == 0x01)
      {
        DIO_Ctrl(IO_AGV_LED_G, DIO_CTRL_ON);
        DIO_Ctrl(IO_AGV_LED_B, DIO_CTRL_OFF);
      }
      if(gLEDFlashMode == 0x02)
      {
        DIO_Ctrl(IO_AGV_LED_B, DIO_CTRL_ON);
        DIO_Ctrl(IO_AGV_LED_G, DIO_CTRL_OFF);
      }
      gLEDFlashModePre = gLEDFlashMode;
    }
    
    switch(gLEDFlashMode)
    {
      case 0x00:// Bootloader ���� �����˸
        DIO_Ctrl(IO_AGV_LED_R, DIO_CTRL_TOGGLE);
        break;
      case 0x01: // ����App������ ����������˸
        DIO_Ctrl(IO_AGV_LED_G, DIO_CTRL_TOGGLE); 
        DIO_Ctrl(IO_AGV_LED_B, DIO_CTRL_TOGGLE); 
        break;
      case 0x02:// ����App�����г���, ��ƽ�����˸
        DIO_Ctrl(IO_AGV_LED_G, DIO_CTRL_TOGGLE);
        break;
      default:
        break;
    }

#else
  App_LEDLocalTest();
#endif
}

#if (APP_LED_LOCAL_TEST_MODE > 0x0)
/**
  * @brief  : LED Ӧ�ñ��ع��ܲ���
  * @note   : ��
  * @param  : �� 
  * @retval : ��
  */
static void App_LEDLocalTest(void)
{
#if (APP_LED_LOCAL_TEST_MODE == 0x01)
  DIO_Ctrl(IO_BOARD_LED_R, DIO_CTRL_TOGGLE);
  osDelay(200);
  DIO_Ctrl(IO_BOARD_LED_G, DIO_CTRL_TOGGLE);
  osDelay(200);
  DIO_Ctrl(IO_BOARD_LED_B, DIO_CTRL_TOGGLE);
  osDelay(200);
#elif (APP_LED_LOCAL_TEST_MODE == 0x02)
  DIO_Ctrl(IO_AGV_LED_R, DIO_CTRL_TOGGLE);
  osDelay(200);
  DIO_Ctrl(IO_AGV_LED_G, DIO_CTRL_TOGGLE);
  osDelay(200);
  DIO_Ctrl(IO_AGV_LED_B, DIO_CTRL_TOGGLE);
  osDelay(200);
#elif (APP_LED_LOCAL_TEST_MODE == 0x03)
  static int8_t count = 0x05;
  while(count > 0x0)
  {
    count--;
    IO_Ctrl(IO_AGV_BEEP, DIO_CTRL_TOGGLE);
    osDelay(200);
  }
  osDelay(100);
#endif
  
}
#endif

/*******************************************END OF FILE***********************************/
