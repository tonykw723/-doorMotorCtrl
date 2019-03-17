/**
  ******************************************************************************
  * @file    ap_action.h
  * @author  tony
  * @version V0.0.1
  * @date    2015-6-6
  * @brief   This file contains the external declaration of ap_action.c file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AP_ACTION_H
#define __AP_ACTION_H

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void DoUp(xt_u8 is_delay);
void DoDown(xt_u8 is_delay);
void DoRelease(void);
xt_u8 Application(void);
xt_u8 HandKeyapplication(void);
#endif /* __AP_ACTION_H */

/*****************************END OF FILE****/


