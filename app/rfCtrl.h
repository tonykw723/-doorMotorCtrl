/**
  ******************************************************************************
  * @file    rfCtrl.h
  * @author  tony
  * @version V0.0.1
  * @date    2015-7-5
  * @brief   This file contains the external declaration of rfCtrl.c file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RF_CTRL_H
#define __RF_CTRL_H

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define PT2264_LOCK 0x0c//0X03
#define PT2264_UP 0x03//0X0C
#define PT2264_UNLOCK 0X30
#define PT2264_DOWN 0XC0

#define EV1527_LOCK 0x08
#define EV1527_UP 0x04
#define EV1527_UNLOCK 0X02
#define EV1527_DOWN 0X01

#define EV1527_LEARN_ADD (EV1527_LOCK+EV1527_UP)//0C
#define EV1527_LEARN_DEL (EV1527_LOCK+EV1527_DOWN)//09
#define PT2264_LEARN_ADD (PT2264_LOCK+PT2264_UP)//0F
#define PT2264_LEARN_DEL (PT2264_LOCK+PT2264_DOWN)//0XCC

#define RF_COMBINE_1th 0D
#define RF_COMBINE_2th 09
#define RF_COMBINE_3th 0b
#define RF_COMBINE_4th 0a


#define RF_TIMEOUT 30
#define NXT_TIMEOUT (4*60)
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	RF_RESET=0,RF_PT2264,RF_EV1527,RF_COMBINE
}rf_e;

/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RfCapPrecess(void);
#endif /* __RF_CTRL_H */

/*****************************END OF FILE****/



