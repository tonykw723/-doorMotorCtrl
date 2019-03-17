/**
  ******************************************************************************
  * @file ap_action.c
  * @brief 
  * @author  tony
  * @version  V0.0.1
  * @date    2015-6-6 
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "xtsys_type.h"
#include "globalVal.h"
#include "protocol.h"
#include "drvUart.h"
#include "ap_action.h"
#include "key.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define DEBUG_ON
/* Private macro -------------------------------------------------------------*/
#ifdef DEBUG_ON
#define	PRINT_DATA printData
#else
#define	PRINT_DATA do{}while(0);
#endif
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
extern recvData_t recvData;
extern sendData_t sendData;
extern flashData_u flashData;
extern xt_u16 Conversion_Value;
extern key_t key[KEY_MAX];

xt_u8 doFuncIdel(void)
{
	return SUCCESS_XT;
}

void delay(xt_u16 timeVal)
{
	xt_u16 i;
	for(;timeVal>0;timeVal--)
	{
		for(i=0;i<5000;i++)
		{
			nop();
		}
	}
}

void DoUp(xt_u8 is_delay)
{
	PRINT_DATA("UP");
	if(flashData.value.isContinueAction)
	{
		if(glbPara.flagPhaseUp==YES)
			return;
	}	
	RELAY_LOCK_OFF();
	if(flashData.value.isUpSwap==NO)
	{
		RELAY_DW_OFF();
		if((glbPara.doTimeOut!=0)&&(flashData.value.isContinueAction)&&is_delay)
		{
			IWDG_ReloadCounter();
			delay(300);
		}
		RELAY_UP_ON();
		RELAY_DOOR_ON();
		glbPara.doorTimeOut=30;
		
	}
	else
	{
		RELAY_UP_OFF();
		if((glbPara.doTimeOut!=0)&&(flashData.value.isContinueAction)&&is_delay)
		{
			IWDG_ReloadCounter();
			delay(300);
		}
		RELAY_DW_ON();
		RELAY_DOOR_ON();
		glbPara.doorTimeOut=30;
	}
	
	
	if(flashData.value.isContinueAction==NO)
	{
		glbPara.doTimeOut=DOT_TIMEOUT;
	}
	else
		glbPara.doTimeOut=CONTINUE_TIMEOUT;
}

xt_u8 doFuncUp(void)
{
	xt_u8 buf[2];
	action_ctrl_t *ctrlIn=(action_ctrl_t *)&recvData.okBuf[sizeof(proHead_t)+1];
	action_ctrl_out_t *ctrlOut=(action_ctrl_out_t*)buf;
	//-pack protocol
	ctrlOut->func=FUNC_UP;
	ctrlOut->para=ctrlIn->para;	
	//PRINT_DATA("FuncUp");
	//-do actioin	
	//RELAY_DOOR_ON();
	//glbPara.doorTimeOut=30;
	if(glbPara.flagPassWordMatchErr)
	{
		sendData.outLen=proPack(&sendData,&recvData,buf,2,CMD_RES_ERR);
	}
	else if(glbPara.flagLock==NO)
	{
		DoUp(YES);
		//glbPara.doTimeOut=DOT_TIMEOUT;
		sendData.outLen=proPack(&sendData,&recvData,buf,2,CMD_RES_OK);
	}
	else
	{
		sendData.outLen=proPack(&sendData,&recvData,buf,2,CMD_RES_LOCKED);
	}
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;
}

void DoDown(xt_u8 is_delay)
{
	PRINT_DATA("DW");
	if(flashData.value.isContinueAction)
	{
		if(glbPara.flagPhaseDown==YES)
			return;
	}	
	
	RELAY_LOCK_OFF();
	if(flashData.value.isUpSwap==NO)
	{
		RELAY_UP_OFF();
		if((glbPara.doTimeOut!=0)&&(flashData.value.isContinueAction))
		{
			IWDG_ReloadCounter();
			delay(300);
		}
		RELAY_DW_ON();
		RELAY_DOOR_ON();
		glbPara.doorTimeOut=30;
	}
	else
	{
		RELAY_DW_OFF();
		if((glbPara.doTimeOut!=0)&&(flashData.value.isContinueAction))
		{
			IWDG_ReloadCounter();
			delay(300);
		}		
		RELAY_UP_ON();
		RELAY_DOOR_ON();
		glbPara.doorTimeOut=30;
	}
	
	if(flashData.value.isContinueAction==NO)
	{
		glbPara.doTimeOut=DOT_TIMEOUT;
	}
	else
		glbPara.doTimeOut=CONTINUE_TIMEOUT;
}

void DoRelease(void)
{
	RELAY_UP_OFF();
	RELAY_DW_OFF();
	RELAY_LOCK_ON();
	RELAY_DOOR_ON();
	glbPara.doorTimeOut=30;
	if(flashData.value.isContinueAction==NO)
	{
		glbPara.doTimeOut=DOT_TIMEOUT;
	}
}


xt_u8 doFuncDown(void)
{
	xt_u8 buf[2];
	action_ctrl_t *ctrlIn=(action_ctrl_t *)&recvData.okBuf[sizeof(proHead_t)+1];
	action_ctrl_out_t *ctrlOut=(action_ctrl_out_t*)buf;
	//-pack protocol
	ctrlOut->func=FUNC_DOWN;
	ctrlOut->para=ctrlIn->para;	
	//PRINT_DATA("FuncDown");
	//-do actioin
	//RELAY_DOOR_ON();
	//glbPara.doorTimeOut=30;
	if(glbPara.flagPassWordMatchErr)
	{
		sendData.outLen=proPack(&sendData,&recvData,buf,2,CMD_RES_ERR);
	}
	else if(glbPara.flagLock==NO)
	{
		DoDown(YES);
		//glbPara.doTimeOut=DOT_TIMEOUT;
		sendData.outLen=proPack(&sendData,&recvData,buf,2,CMD_RES_OK);
	}
	else
	{
		sendData.outLen=proPack(&sendData,&recvData,buf,2,CMD_RES_LOCKED);
	}
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;
}

xt_u8 doFuncUnlock(void)
{
	xt_u8 buf[1];

	//-do actioin
	RELAY_UP_OFF();
	RELAY_DW_OFF();
	RELAY_LOCK_ON();
	RELAY_DOOR_ON();
	glbPara.doorTimeOut=30;	
	//-pack protocol
	buf[0]=FUNC_UNLOCK;
	glbPara.flagLock=NO;
	glbPara.doTimeOut=DOT_TIMEOUT;
	//PRINT_DATA("FuncUnlock");
	if(glbPara.flagPassWordMatchErr)
	{
		sendData.outLen=proPack(&sendData,&recvData,buf,1,CMD_RES_ERR);
	}
	else 
		sendData.outLen=proPack(&sendData,&recvData,buf,1,CMD_RES_OK);
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;
}

xt_u8 doFuncLock(void)
{
	xt_u8 buf[1];

	//-do actioin
	RELAY_UP_OFF();
	RELAY_DW_OFF();
	RELAY_LOCK_ON();
	RELAY_DOOR_ON();
	glbPara.doorTimeOut=30;
	//-pack protocol
	buf[0]=FUNC_LOCK;
	glbPara.flagLock=YES;
        glbPara.doTimeOut=DOT_TIMEOUT;
	//PRINT_DATA("FuncLock");
	if(glbPara.flagPassWordMatchErr)
	{
		sendData.outLen=proPack(&sendData,&recvData,buf,1,CMD_RES_ERR);
	}
	else 
		sendData.outLen=proPack(&sendData,&recvData,buf,1,CMD_RES_OK);
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;
}

xt_u8 doFuncToLearn(void)
{
	xt_u8 buf[1];

	//-do actioin
	key[KEY_LEARN].val=DOT_PRESS;//模拟按键按下
	//-pack protocol
	buf[0]=FUNC_TO_LEARN;
	//PRINT_DATA("doFuncToLearn");
	if(glbPara.flagPassWordMatchErr)
	{
		sendData.outLen=proPack(&sendData,&recvData,buf,1,CMD_RES_ERR);
	}
	else 
		sendData.outLen=proPack(&sendData,&recvData,buf,1,CMD_RES_OK);
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;
}

xt_u8 asc2hex(xt_u8 *pt)
{
	xt_u8 temp;
	temp=pt[0]-'0';
	temp<<=4;
	temp|=pt[1]-'0';
	return temp;
}

xt_u8 doFuncPassWord(void)
{
	xt_u8 buf[1];
	xt_u8 headLen=sizeof(proHead_t);
	xt_u8 dataLen= recvData.okBuf[headLen];
	xt_u8 cmdRes=CMD_RES_OK;
	xt_u16 uidT=0;
        xt_u8 i;
	
	//-do actioin
	//-pack protocol
	buf[0]=FUNC_PASSWORD;
	if(glbPara.flagPassWordMatchErr)
	{
		cmdRes=CMD_RES_ERR;
	}
	else if(dataLen<=16)
	{
		for(i=0;(i<16)&&(recvData.okBuf[headLen+i+1]!=0);i++)
		{
			if((recvData.okBuf[headLen+i+1]>='0')&&(recvData.okBuf[headLen+i+1]<='9'))
			{
			}
			else
			{
				cmdRes=CMD_RES_ERR;
				break;
			}
		}	
		if(cmdRes==CMD_RES_OK)
		{
			#if 1
			glbPara.flagPassWord=YES;
			i=0;
			flashData.value.password[i++]=asc2hex(&recvData.okBuf[headLen+1]);
			flashData.value.password[i++]=asc2hex(&recvData.okBuf[headLen+3]);
			flashData.value.password[i++]=asc2hex(&recvData.okBuf[headLen+5]);
			flashData.value.password[i++]=asc2hex(&recvData.okBuf[headLen+7]);
			flashData.value.password[i++]=asc2hex(&recvData.okBuf[headLen+9]);
			flashData.value.password[i++]=asc2hex(&recvData.okBuf[headLen+11]);
			flashData.value.password[i++]=asc2hex(&recvData.okBuf[headLen+13]);
			flashData.value.password[i++]=asc2hex(&recvData.okBuf[headLen+15]);			
			#endif
			flashData.value.uidAddr=uidT;
			flashData.value.haveWrited=0x1234;
			//--to wr to flash
			writeFlashData();
		}
	}
	else
	{
		glbPara.flagPassWord=NO;
		cmdRes=CMD_RES_ERR;
	}
	//PRINT_DATA("doFuncPassWord");
	sendData.outLen=proPack(&sendData,&recvData,buf,1,cmdRes);
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;
}

xt_u8 doFuncChangeUpToDown(void)
{/*
	xt_u8 buf[1];

	//-do actioin
	//-pack protocol
	buf[0]=FUNC_CHANGE_UP_TO_DOWN;
	//PRINT_DATA("doFuncChangeUpToDown");
	sendData.outLen=proPack(&sendData,&recvData,buf,1,CMD_RES_OK);
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;*/
}

xt_u8 doFuncChangeDotToCont(void)
{/*
	xt_u8 buf[1];

	//-do actioin
	//-pack protocol
	buf[0]=FUNC_CHANGE_DOT_TO_CONT;
	//PRINT_DATA("doFuncChangeDotToCont");
	sendData.outLen=proPack(&sendData,&recvData,buf,1,CMD_RES_OK);
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;*/
}

xt_u8 doFuncMatchPassword(void)
{
	xt_u8 buf[1];
	xt_u8 headLen=sizeof(proHead_t);
	xt_u8 dataLen= recvData.okBuf[headLen];
	xt_u8 cmdRes=CMD_RES_OK;
	xt_u16 uidT=0;
	//-do actioin
	//-pack protocol
	buf[0]=FUNC_MATCH_PASSWORD;
	if(glbPara.flagPassWordMatchErr)
	{
		cmdRes=CMD_RES_ERR;
		glbPara.flagPassWordMatchErr=NO;
	}
	else if(dataLen<=16)
	{
		for(xt_u8 i=0;i<4;i++)
		{
			if((recvData.okBuf[headLen+i+1]>='0')&&(recvData.okBuf[headLen+i+1]<='9'))
			{
			}
			else
			{
				cmdRes=CMD_RES_ERR;
				break;
			}
		}
		if(cmdRes==CMD_RES_OK)
		{
			if(flashData.value.password[0]!=asc2hex(&recvData.okBuf[headLen+1])||
			flashData.value.password[1]!=asc2hex(&recvData.okBuf[headLen+3])||
			flashData.value.password[2]!=asc2hex(&recvData.okBuf[headLen+5])||
			flashData.value.password[3]!=asc2hex(&recvData.okBuf[headLen+7])||
			flashData.value.password[4]!=asc2hex(&recvData.okBuf[headLen+9])||
			flashData.value.password[5]!=asc2hex(&recvData.okBuf[headLen+11])||
			flashData.value.password[6]!=asc2hex(&recvData.okBuf[headLen+13])||
			flashData.value.password[7]!=asc2hex(&recvData.okBuf[headLen+15]))
				cmdRes=CMD_RES_ERR;

		}
	}
	else
	{
		//PRINT_DATA("doFuncMatchPassword");
		cmdRes=CMD_RES_ERR;
	}
	
	sendData.outLen=proPack(&sendData,&recvData,buf,1,cmdRes);
	//-to send protocol
	uartSend(sendData.outBuf,sendData.outLen);
	return SUCCESS_XT;
}

xt_u8 (*action[id_max])(void)=
{
	doFuncIdel,
	doFuncUp,
	doFuncDown,
	doFuncUnlock,
	doFuncLock,	
	doFuncToLearn,
	doFuncPassWord,
	doFuncChangeUpToDown,
	doFuncChangeDotToCont,
	doFuncMatchPassword
};


xt_u8 getFuncId(xt_u8 *okBuf)
{
	xt_u8 funcId=id_idle;
	proHead_t *head=(proHead_t *)okBuf;
	switch(head->func)
	{
		case FUNC_UP :
			funcId=id_up;
			break;
		case FUNC_DOWN:
			funcId=id_down;
			break;
		case FUNC_UNLOCK:
			funcId=id_unlock;
			break;
		case FUNC_LOCK:
			funcId=id_lock;
			break;
                
                case FUNC_TO_LEARN:
			funcId=id_toLearn;
			break;
		case FUNC_PASSWORD:
			funcId=id_password;
			break;
		case FUNC_CHANGE_UP_TO_DOWN:
			funcId=id_ChangeUpToDown;
			break;
		case FUNC_CHANGE_DOT_TO_CONT:
			funcId=id_ChangeDotToCont;
			break;
		case FUNC_MATCH_PASSWORD:
			funcId=id_matchPassword;
			break;
		default:
			funcId=id_idle;
			break;
	}
	return funcId;
}
xt_u8 Application(void)
{
	xt_u8 funcId=id_idle;
	funcId=getFuncId(recvData.okBuf);
	if(funcId!=id_idle)
	{
		(*action[funcId])();
		funcId=id_idle;
		
	}
        return 1;
}
/*
xt_u8 HandKeyapplication(void)
{
	xt_u8 funcId=id_idle;
	funcId=GetHandKey();
	if(funcId!=id_idle)
	{
		(*action[funcId])();
		
	}
}
*/


