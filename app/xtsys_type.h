/**
  ******************************************************************************
  * @file    board.h
  * @author  tony
  * @version V0.0.1
  * @date    2015-6-6
  * @brief   This file contains the external declaration of board.c file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __XTSYS_TYPE_H_
#define __XTSYS_TYPE_H_

/* Includes ------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

typedef signed char xt_s8;
typedef unsigned char xt_u8;
typedef int xt_s16;
typedef unsigned int xt_u16;
typedef long xt_s32;
typedef unsigned long xt_u32;
typedef long long xt_s64;
typedef char * xt_string;
typedef unsigned char xt_bool;
typedef void xt_void;
typedef float xt_float;
typedef double xt_double;
typedef xt_u32 xt_time_t;


//#define NULL (0)

#define TRUE 1
#define FALSE 0

#define YES 1
#define NO 0

#define SUCCESS_XT 1
#define FAILURE_XT 0
#define WAIT_XT 2

#endif /* __XTSYS_TYPE */

/*****************************END OF FILE****/


