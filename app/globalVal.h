/**
  ******************************************************************************
  * @file    board.h
  * @author  tony
  * @version V0.0.1
  * @date    2015-6-6
  * @brief   This file contains the external declaration of common file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GLOBALVAL_H
#define __GLOBALVAL_H

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BL2_0
//#define BLE

//#define USE_12M
#define USE_16M

//#define HAND_BORAD
//#define FIRST_VER
#define SECOND_VER

#define CONVERSE
#define OLD_RF

#define BLOCK_OPERATION    0    /* block 0 in data eeprom memory: address is 0x4000 */
#define FLASH_DATA_MAX 128
#define FLASH_START_ADDR FLASH_DATA_START_PHYSICAL_ADDRESS


#define IN_MAX 60
#define CHK_MAX 60
#define OK_MAX 60
#define DATA_MAX 30
#define OUT_MAX 60

#define BAUDRATE 9600
#define RF_CODE_MAX 25

#define DOT_TIMEOUT 30//(1*100)
#define CONTINUE_TIMEOUT (240*100)
#define LEARN_TIMEOUT 10

//-LED 
//#if defined(FIRST_VER)||defined (HAND_BORAD)
//#define LED_RUN_GPIO_PORT  (GPIOA)
//#define LED_RUN_GPIO_PIN  (GPIO_PIN_3)
//#elif defined(SECOND_VER)
#define LED_RUN_GPIO_PORT  (GPIOA)
#define LED_RUN_GPIO_PIN  (GPIO_PIN_3)
//#endif
#define LED_RUN_H() LED_RUN_GPIO_PORT->ODR |= (uint8_t)LED_RUN_GPIO_PIN//GPIO_WriteHigh(LED_RUN_GPIO_PORT, (GPIO_Pin_TypeDef)LED_RUN_GPIO_PIN)
#define LED_RUN_L() LED_RUN_GPIO_PORT->ODR &= (uint8_t)(~LED_RUN_GPIO_PIN)//GPIO_WriteLow(LED_RUN_GPIO_PORT, (GPIO_Pin_TypeDef)LED_RUN_GPIO_PIN)
#define LED_RUN_TOGGLE() LED_RUN_GPIO_PORT->ODR ^= (uint8_t)LED_RUN_GPIO_PIN//GPIO_WriteReverse(LED_RUN_GPIO_PORT, (GPIO_Pin_TypeDef)LED_RUN_GPIO_PIN)

//-RELAY 
#if defined( HAND_BORAD)
#define RELAY_UP_GPIO_PORT  (GPIOC)
#define RELAY_UP_GPIO_PIN  (GPIO_PIN_5)
#define RELAY_UP_OFF() RELAY_UP_GPIO_PORT->ODR |= (uint8_t)RELAY_UP_GPIO_PIN//GPIO_WriteHigh(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_UP_ON() RELAY_UP_GPIO_PORT->ODR &= (uint8_t)(~RELAY_UP_GPIO_PIN)//GPIO_WriteLow(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)

#define RELAY_DW_GPIO_PORT  (GPIOC)
#define RELAY_DW_GPIO_PIN  (GPIO_PIN_4)
#define RELAY_DW_OFF() RELAY_DW_GPIO_PORT->ODR |= (uint8_t)RELAY_DW_GPIO_PIN//GPIO_WriteHigh(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_DW_ON() RELAY_DW_GPIO_PORT->ODR &= (uint8_t)(~RELAY_DW_GPIO_PIN)//GPIO_WriteLow(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)

#define RELAY_LOCK_GPIO_PORT  (GPIOB)
#define RELAY_LOCK_GPIO_PIN  (GPIO_PIN_5)
#define RELAY_LOCK_OFF() RELAY_LOCK_GPIO_PORT->ODR |= (uint8_t)RELAY_LOCK_GPIO_PIN//GPIO_WriteHigh(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_LOCK_ON() RELAY_LOCK_GPIO_PORT->ODR &= (uint8_t)(~RELAY_LOCK_GPIO_PIN)//GPIO_WriteLow(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#elif defined(FIRST_VER)
#define RELAY_UP_GPIO_PORT  (GPIOC)
#define RELAY_UP_GPIO_PIN  (GPIO_PIN_3)
#define RELAY_DW_GPIO_PORT  (GPIOB)
#define RELAY_DW_GPIO_PIN  (GPIO_PIN_4)
#define RELAY_LOCK_GPIO_PORT  (GPIOB)
#define RELAY_LOCK_GPIO_PIN  (GPIO_PIN_5)
	#ifndef CONVERSE
#define RELAY_UP_OFF() RELAY_UP_GPIO_PORT->ODR |= (uint8_t)RELAY_UP_GPIO_PIN//GPIO_WriteHigh(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_UP_ON() RELAY_UP_GPIO_PORT->ODR &= (uint8_t)(~RELAY_UP_GPIO_PIN)//GPIO_WriteLow(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_DW_OFF() RELAY_DW_GPIO_PORT->ODR |= (uint8_t)RELAY_DW_GPIO_PIN//GPIO_WriteHigh(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_DW_ON() RELAY_DW_GPIO_PORT->ODR &= (uint8_t)(~RELAY_DW_GPIO_PIN)//GPIO_WriteLow(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_LOCK_OFF() RELAY_LOCK_GPIO_PORT->ODR |= (uint8_t)RELAY_LOCK_GPIO_PIN//GPIO_WriteHigh(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_LOCK_ON() RELAY_LOCK_GPIO_PORT->ODR &= (uint8_t)(~RELAY_LOCK_GPIO_PIN)//GPIO_WriteLow(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
	#else
#define RELAY_UP_ON() RELAY_UP_GPIO_PORT->ODR |= (uint8_t)RELAY_UP_GPIO_PIN//GPIO_WriteHigh(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_UP_OFF() RELAY_UP_GPIO_PORT->ODR &= (uint8_t)(~RELAY_UP_GPIO_PIN)//GPIO_WriteLow(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_DW_ON() RELAY_DW_GPIO_PORT->ODR |= (uint8_t)RELAY_DW_GPIO_PIN//GPIO_WriteHigh(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_DW_OFF() RELAY_DW_GPIO_PORT->ODR &= (uint8_t)(~RELAY_DW_GPIO_PIN)//GPIO_WriteLow(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_LOCK_OFF() RELAY_LOCK_GPIO_PORT->ODR &= (uint8_t)(~RELAY_LOCK_GPIO_PIN)//GPIO_WriteLow(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_LOCK_ON() RELAY_LOCK_GPIO_PORT->ODR |= (uint8_t)RELAY_LOCK_GPIO_PIN//GPIO_WriteHigh(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
	#endif
#elif defined(SECOND_VER)

#define RELAY_UP_GPIO_PORT  (GPIOC)
#define RELAY_UP_GPIO_PIN  (GPIO_PIN_5)
#define RELAY_DW_GPIO_PORT  (GPIOC)
#define RELAY_DW_GPIO_PIN  (GPIO_PIN_4)
#define RELAY_LOCK_GPIO_PORT  (GPIOC)
#define RELAY_LOCK_GPIO_PIN  (GPIO_PIN_3)
#define RELAY_DOOR_GPIO_PORT  (GPIOA)
#define RELAY_DOOR_GPIO_PIN  (GPIO_PIN_3)
	#ifndef CONVERSE
#define RELAY_UP_OFF() RELAY_UP_GPIO_PORT->ODR |= (uint8_t)RELAY_UP_GPIO_PIN//GPIO_WriteHigh(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_UP_ON() RELAY_UP_GPIO_PORT->ODR &= (uint8_t)(~RELAY_UP_GPIO_PIN)//GPIO_WriteLow(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_DW_OFF() RELAY_DW_GPIO_PORT->ODR |= (uint8_t)RELAY_DW_GPIO_PIN//GPIO_WriteHigh(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_DW_ON() RELAY_DW_GPIO_PORT->ODR &= (uint8_t)(~RELAY_DW_GPIO_PIN)//GPIO_WriteLow(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_LOCK_OFF() RELAY_LOCK_GPIO_PORT->ODR |= (uint8_t)RELAY_LOCK_GPIO_PIN//GPIO_WriteHigh(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_LOCK_ON() RELAY_LOCK_GPIO_PORT->ODR &= (uint8_t)(~RELAY_LOCK_GPIO_PIN)//GPIO_WriteLow(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_DOOR_OFF() RELAY_DOOR_GPIO_PORT->ODR |= (uint8_t)RELAY_DOOR_GPIO_PIN//GPIO_WriteHigh(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_DOOR_ON() RELAY_DOOR_GPIO_PORT->ODR &= (uint8_t)(~RELAY_DOOR_GPIO_PIN)//GPIO_WriteLow(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)

	#else
#define RELAY_UP_ON() RELAY_UP_GPIO_PORT->ODR |= (uint8_t)RELAY_UP_GPIO_PIN//GPIO_WriteHigh(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_UP_OFF() RELAY_UP_GPIO_PORT->ODR &= (uint8_t)(~RELAY_UP_GPIO_PIN)//GPIO_WriteLow(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN)
#define RELAY_DW_ON() RELAY_DW_GPIO_PORT->ODR |= (uint8_t)RELAY_DW_GPIO_PIN//GPIO_WriteHigh(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_DW_OFF() RELAY_DW_GPIO_PORT->ODR &= (uint8_t)(~RELAY_DW_GPIO_PIN)//GPIO_WriteLow(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN)
#define RELAY_LOCK_OFF() RELAY_LOCK_GPIO_PORT->ODR &= (uint8_t)(~RELAY_LOCK_GPIO_PIN)//GPIO_WriteLow(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_LOCK_ON() RELAY_LOCK_GPIO_PORT->ODR |= (uint8_t)RELAY_LOCK_GPIO_PIN//GPIO_WriteHigh(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_DOOR_OFF() nop()//RELAY_DOOR_GPIO_PORT->ODR &= (uint8_t)(~RELAY_DOOR_GPIO_PIN)//GPIO_WriteLow(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)
#define RELAY_DOOR_ON() nop()//RELAY_DOOR_GPIO_PORT->ODR |= (uint8_t)RELAY_DOOR_GPIO_PIN//GPIO_WriteHigh(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN)

	#endif

#endif



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	xt_u8 recvTimeOut;
	xt_u8 recvStartIdx;
	xt_u8 recvNextIdx;
	xt_u16 msCnt;
	xt_u16 doTimeOut;
	xt_u16 rfTimeOut;
	xt_u16 nxtActionTimeOut;
	xt_u8 doorTimeOut;
	xt_u8 learnTimeOut;
	xt_u8 passWord[8];
	
	xt_u16 flagOneSec:1;
	xt_u16 flagLock:1;
	xt_u16 flagLearn:1;
	xt_u16 flagDel:1;
	xt_u16 flagPhaseUp:1;
	xt_u16 flagPhaseDown:1;
	xt_u16 flagPassWord:1;
	xt_u16 flagBTDisconnect:1;
	xt_u16 flagPassWordSetOk:1;
	xt_u16 flagPassWordMatchErr:1;
	//xt_u8 funcId;
}runningPara_t;

typedef struct
{
	xt_u8 inBuf[IN_MAX];
	xt_u8 chkBuf[CHK_MAX];
	xt_u8 okBuf[OK_MAX];
	//xt_u8 dataBuf[DATA_MAX];
	//xt_u8 *pDataBuf;
	
	xt_u8 inLen;
	xt_u8 chkLen;
	xt_u8 okLen;
	//xt_u8 dataLen;
}recvData_t;

typedef struct
{
	xt_u8 outBuf[OUT_MAX];
	//xt_u8 dataBuf[DATA_MAX];
	xt_u8 outLen;
	//xt_u8 dataLen;	
}sendData_t;

typedef struct
{
	xt_u16 haveWrited;
	xt_u16 uidAddr;
	xt_u16 baudRate;//96:9600,1152:115200
	xt_u8 isUpSwap:1;
	xt_u8 isContinueAction:1;
	xt_u8 rfCodeIdx;
	xt_u32 rfCode[RF_CODE_MAX];
	xt_u8 password[8]; 
	
}flashData_t;

typedef union
{
	xt_u8 buf[FLASH_DATA_MAX];
	flashData_t value;
}flashData_u;

typedef struct
{

	#ifdef OLD_RF
	xt_u16 buf[2][48];
	#else
	xt_u16 buf[98];
	#endif
	xt_u8 start;
	xt_u8 finished;
	xt_u8 timeout;
	xt_u8 codeIdx;	
}rfData_t;

/* Exported types ------------------------------------------------------------*/
extern runningPara_t glbPara;
extern recvData_t recvData;
extern sendData_t sendData;
/* Exported functions ------------------------------------------------------- */
void printData(xt_u8 *pt);
void printByteData(xt_u8 *pt,xt_u8 size);

#endif /* __GLOBALVAL_H */

/*****************************END OF FILE****/


