/**
  ******************************************************************************
  * @file    drvUart.h
  * @author  tony
  * @version V0.0.1
  * @date    2015-6-6
  * @brief   This file contains the external declaration of drvUart.c file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRVUART_H
#define __DRVUART_H

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void DebugUart_SendData8(xt_u8 ch);
xt_u8 uartRecv(void);
void uartSend(xt_u8*pt,xt_u8 size);
void uartPackage(xt_u8*pt,xt_u8 size);

#endif /* __DRVUART_H */

/*****************************END OF FILE****/


