/**
  ******************************************************************************
  * @file    key.h
  * @author  tony
  * @version V0.0.1
  * @date    2015-7-5
  * @brief   This file contains the external declaration of key.c file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_H
#define __KEY_H

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//-KEY
#if defined( HAND_BORAD)
#define KEY_LEARN_GPIO_PORT  (GPIOC)
#define KEY_LEARN_GPIO_PIN  (GPIO_PIN_3)
#define KEY_SWAP_GPIO_PORT  (GPIOB)
#define KEY_SWAP_GPIO_PIN  (GPIO_PIN_4)
#define KEY_LEARN_GET() GPIO_ReadInputPin(KEY_LEARN_GPIO_PORT, KEY_LEARN_GPIO_PIN)
#define KEY_SWAP_GET() GPIO_ReadInputPin(KEY_SWAP_GPIO_PORT, KEY_SWAP_GPIO_PIN)
#elif defined(FIRST_VER)
#define KEY_LEARN_GPIO_PORT  (GPIOC)
#define KEY_LEARN_GPIO_PIN  (GPIO_PIN_5)
#define KEY_SWAP_GPIO_PORT  (GPIOC)
#define KEY_SWAP_GPIO_PIN  (GPIO_PIN_7)
#define KEY_LEARN_GET() GPIO_ReadInputPin(KEY_LEARN_GPIO_PORT, KEY_LEARN_GPIO_PIN)
#define KEY_SWAP_GET() GPIO_ReadInputPin(KEY_SWAP_GPIO_PORT, KEY_SWAP_GPIO_PIN)
#elif defined(SECOND_VER)
#define KEY_LEARN_GPIO_PORT  (GPIOB)
#define KEY_LEARN_GPIO_PIN  (GPIO_PIN_4)
#define KEY_LEARN_GET() GPIO_ReadInputPin(KEY_LEARN_GPIO_PORT, KEY_LEARN_GPIO_PIN)

#define CHK_PHASE1_GPIO_PORT  (GPIOD)
#define CHK_PHASE1_GPIO_PIN  (GPIO_PIN_4)
#define CHK_PHASE1_GET() GPIO_ReadInputPin(CHK_PHASE1_GPIO_PORT, CHK_PHASE1_GPIO_PIN)

#define CHK_PHASE2_GPIO_PORT  (GPIOD)
#define CHK_PHASE2_GPIO_PIN  (GPIO_PIN_2)
#define CHK_PHASE2_GET() GPIO_ReadInputPin(CHK_PHASE2_GPIO_PORT, CHK_PHASE2_GPIO_PIN)
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

typedef enum
{
	KEY_LEARN=0,
	//KEY_SWAP,
	KEY_MAX
}key_e;

typedef enum
{
	NONE_PRESS=0,DOT_PRESS,CONTINUE_PRESS
}press_e;

typedef struct
{
	xt_u8 step;
	xt_u8 (*func)(void);
	xt_u8 cnt;
	press_e val;
}key_t;

/* Exported functions ------------------------------------------------------- */
void KEY_delRfCode(void);
void GetKeyProcess(void);
void GetHandKey(void);
#endif /* __RF_CTRL_H */

/*****************************END OF FILE****/




