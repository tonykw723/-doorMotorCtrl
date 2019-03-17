/**
  ******************************************************************************
  * @file    protocol.h
  * @author  tony
  * @version V0.0.1
  * @date    2015-6-6
  * @brief   This file contains the external declaration of protocol.c file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PROTOCOL_H
#define __PROTOCOL_H

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CMD_SET 0XA0
#define CMD_CLR 0XA1
#define CMD_GET 0XA2
#define CMD_RES_OK 0X80  //respond success
#define CMD_RES_ERR 0X81  //respond failure
#define CMD_RES_LOCKED 0X82  //respond failure

#define FUNC_UP 0X41
#define FUNC_DOWN 0X42
#define FUNC_UNLOCK 0X43
#define FUNC_LOCK 0X44
#define FUNC_TO_LEARN 0X45
#define FUNC_PASSWORD 0X46
#define FUNC_CHANGE_UP_TO_DOWN 0X47
#define FUNC_CHANGE_DOT_TO_CONT 0X48
#define FUNC_MATCH_PASSWORD 0X49

/* Private macro -------------------------------------------------------------*/
typedef enum
{
	id_idle=0,
	id_up,
	id_down,
	id_unlock,
	id_lock,
	id_toLearn,
	id_password,
	id_ChangeUpToDown,
	id_ChangeDotToCont,
	id_matchPassword,
	id_max,
}funcId_e;

typedef enum
{
	ONE_STEP=1,CONTINUE_STEP=2
}doStep_e;
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	xt_u16 head;
	xt_u8 addr[2];
	xt_u8 password[8];
	xt_u8 cmd;
	xt_u8 func;
	//xt_u8 dataLen;
	//xt_u8 *pData;
}proHead_t;


typedef struct
{
	xt_u8 para;
}action_ctrl_t;

typedef struct
{
	xt_u8 func;
	xt_u8 para;
}action_ctrl_out_t;


/* Exported types ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
xt_u8 proPack(sendData_t *sendData,recvData_t *recvData,xt_u8 *pDataBuf,xt_u8 dataLen,xt_u8 cmdRes);
xt_u8 proParse(recvData_t *recvData);


#endif /* __PROTOCOL_H */

/*****************************END OF FILE****/


