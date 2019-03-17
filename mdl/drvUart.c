/**
  ******************************************************************************
  * @file drvUart.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm8s.h"
#include "xtsys_type.h"
#include "globalVal.h"
#include "board.h"
#include "drvUart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEBUG_ON
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
recvData_t recvData;
sendData_t sendData;

/**
  * @brief  
  * @param  None
  * @retval None
  */
void printData(xt_u8 *pt)
{
	xt_u16 i=0;
	if(pt==NULL)
		return;
	for(i=0;pt[i]!=0;i++)
	{
		DebugUart_SendData8(pt[i]);
	}
	DebugUart_SendData8(0x0d);
	DebugUart_SendData8(0x0a);
}

void printByteData(xt_u8 *pt,xt_u8 size)
{
	xt_u16 i=0;
	xt_u8 temp;
	if(pt==NULL)
		return;
	
	for(i=0;i<size;i++)
	{
		temp = pt[i]>>4;
		if(temp>9)
		{
			DebugUart_SendData8(temp+'a');
		}
		else
		{
			DebugUart_SendData8(temp+'0');
		}

		temp = pt[i]&0x0f;
		if(temp>9)
		{
			DebugUart_SendData8(temp+'a');
		}
		else
		{
			DebugUart_SendData8(temp+'0');
		}		

		DebugUart_SendData8(' ');
	}
	DebugUart_SendData8(0x0d);
	DebugUart_SendData8(0x0a);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
xt_u8 uartRecv(void)
{
	xt_u8 i,j;
	if(glbPara.recvStartIdx==glbPara.recvNextIdx)
		return FAILURE_XT;
	if(glbPara.recvTimeOut!=0)
		return FAILURE_XT;
	UART1_Cmd(DISABLE);
	//---get chk buf
	//      recvStartIdx | recvNextIdx
	//if           0           |      31
	//if           31         |       80=0
	//if           80=0     |       30
	//if           30         |       79
	//if           79         |       30		
	//PRINT_DATA("recv:");
	for(i=glbPara.recvStartIdx,j=0;j<recvData.inLen;j++)
	{
		if(j>=CHK_MAX)
		break;
		DebugUart_SendData8(recvData.inBuf[i]);
		if(i>=IN_MAX-1)// if 79>=IN_MAX(80)-1
		{
			recvData.chkBuf[j]=recvData.inBuf[i];
			i=0;
			continue;
		}

		recvData.chkBuf[j]=recvData.inBuf[i];
		i++;
	}
	recvData.chkBuf[j]=0;
	recvData.chkLen=j;
	//PRINT_DATA(recvData.chkLen);
	//---renew StartIdx
	if(glbPara.recvNextIdx>=IN_MAX)// if =IN_MAX clr 0
	{
		glbPara.recvNextIdx=0;
	}
	glbPara.recvStartIdx=glbPara.recvNextIdx;
	recvData.inLen=0;
	UART1_Cmd(ENABLE);
	//---chk Buf info
	//PRINT_DATA("chkBuf:");
	printByteData(recvData.chkBuf,recvData.chkLen);
	LED_flash();
	return SUCCESS_XT;
	
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
void uartSend(xt_u8*pt,xt_u8 size)
{
	xt_u8 i;
	PRINT_DATA("outBuf:");
	printByteData(pt,size);
	for(i=0;i<size;i++)
	{
		UART1->DR = pt[i];//UART1_SendData8(pt[i]);
		while((UART1->SR & (uint8_t)UART1_FLAG_TXE)==RESET);
	}
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void uartPackage(xt_u8*pt,xt_u8 size)
{

	memcpy(sendData.outBuf,pt,size);
	sendData.outBuf[size++]=0x0d;
	sendData.outBuf[size++]=0x0a;
	sendData.outLen=size;

}