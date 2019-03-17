/**
  ******************************************************************************
  * @file protocol.c
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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
extern flashData_u flashData;


xt_u8 proPack(sendData_t *sendData,recvData_t *recvData,xt_u8 *pDataBuf,xt_u8 dataLen,xt_u8 cmdRes)
{
	xt_u8 headLen=sizeof(proHead_t);
	xt_u8 len;
	xt_u8 chkSum=0;
	xt_u8 i;
	
	memcpy(sendData->outBuf,recvData->okBuf,headLen);
	sendData->outBuf[0]=0xbb;
	sendData->outBuf[1]=0xbb;
	sendData->outBuf[5+8]=cmdRes;//respond cmd
	sendData->outBuf[headLen]=dataLen;
	memcpy(&sendData->outBuf[headLen+1],pDataBuf,dataLen);
	len=dataLen+7+8;
	for(i=2;i<len;i++)
	{
		chkSum+=sendData->outBuf[i];
	}
	sendData->outBuf[i++]=chkSum;
	sendData->outBuf[i++]=0xee;
	len=i;
	return len;
}


xt_u8 stringCmp(xt_u8 *pt,xt_u8 *forCmp,xt_u8 size)
{
	xt_u8 i;
	for(i=0;i<size;i++)
	{
		if(pt[i]!=forCmp[i])
		{
			return FAILURE_XT;
		}
	}
	return SUCCESS_XT;
}

xt_u8 proParse(recvData_t *recvData)
{
	xt_u8 ret=FAILURE_XT;
	xt_u8 step=0;
	xt_u8 dataLen=0;
	xt_u8 proLen=0;
	xt_u8 chkSum=0;
	xt_u8 i=0,j=0;
	xt_u8 i_std=0;
	xt_u8 i_dataLen=0;
	xt_u8 i_chkSum=0;
	xt_u8 i_end=0;
	
	if(recvData->chkLen<2)
		return ret;
	
	for(j=0;j<recvData->chkLen;j++)
	{
	switch(step)
	{
		case 0:
			for(i=i_std;i<recvData->chkLen;i++)
			{
				if((recvData->chkBuf[i]==0xaa)&&(recvData->chkBuf[i+1]==0xaa))
				{
					i_std=i;
					step=1;
					break;
				}
				#if 0
				else if((recvData->chkBuf[i]=='+'))
				{
					if(stringCmp(&recvData->chkBuf[i],"+DISC:",sizeof("+DISC:")-1))
					{	
						glbPara.flagBTDisconnect=YES;
						return FAILURE_XT;
					}
					else if(stringCmp(&recvData->chkBuf[i],"+CONNECTED",sizeof("+CONNECTED")-1))
					{	
						glbPara.flagBTDisconnect=NO;
						return FAILURE_XT;
					}
				}
				else if((recvData->chkBuf[i]=='O')&&(recvData->chkBuf[i+1]=='K'))
				{
					if(glbPara.flagPassWord)
					{
						//glbPara.flagPassWordSetOk=YES;
						glbPara.flagPassWord=NO;
						return FAILURE_XT;
					}
				}
				#endif
			}
			if(i==recvData->chkLen)
				return FAILURE_XT;
			break;
		case 1:
			i_dataLen=i_std+14;//6+8;
			dataLen=recvData->chkBuf[i_dataLen];
			proLen=dataLen+17;//9+8;
			i_chkSum=proLen-2;
			i_end=proLen-1;
			step=2;
			break;
		case 2:
			if(0xee!=recvData->chkBuf[i_end])//格式错，从0xaa的下一个字节开始解析。
			{
				i_std++;
				j=i_std;
				step=0;
				continue;
			}
			for(i=i_std+2;i<i_end-1;i++)
			{
				chkSum+=recvData->chkBuf[i];
			}
			//if(chkSum==recvData->chkBuf[i_chkSum])
			{
				//xt_u16 uidTemp=0;
				for(i=0;i<proLen;i++)
				{
					recvData->okBuf[i]=recvData->chkBuf[i_std+i];
				}
				//uidTemp|=recvData->okBuf[2];
				//uidTemp<<=8;
				//uidTemp|=recvData->okBuf[3];
				
				if((recvData->okBuf[4]!=flashData.value.password[0])||
					(recvData->okBuf[5]!=flashData.value.password[1])||
					(recvData->okBuf[6]!=flashData.value.password[2])||
					(recvData->okBuf[7]!=flashData.value.password[3])||
					(recvData->okBuf[8]!=flashData.value.password[4])||
					(recvData->okBuf[9]!=flashData.value.password[5])||
					(recvData->okBuf[10]!=flashData.value.password[6])||
					(recvData->okBuf[11]!=flashData.value.password[7]))
				{
					//if(recvData->okBuf[13]!=FUNC_MATCH_PASSWORD)
					//	return FAILURE_XT;
					//else
						glbPara.flagPassWordMatchErr=YES;
				}
                                else
                                   glbPara.flagPassWordMatchErr=NO;
				recvData->okLen=proLen;
				//for(i=0;i<dataLen;i++)
				//{
				//	recvData->dataBuf[i]=recvData->chkBuf[i_dataLen+1+i];
				//}
				//recvData->dataLen=dataLen;
				//PRINT_DATA("okBuf:");
				//printByteData(recvData->okBuf,recvData->okLen);
				//PRINT_DATA("dataBuf:");
				//PRINT_DATA(recvData->dataBuf);		
				//recvData->pDataBuf=&recvData->chkBuf[i_dataLen+1+i];
				//recvData->dataLen=dataLen;
				
				return SUCCESS_XT;
			}
			/*
			else //格式对，校验错，从0xee的下一个字节开始解析
			{
				PRINT_DATA("chSum :");
				printByteData(&chkSum,1);
				i_std=i_end;
				j=i_std;
				step=0;
				continue;
			}*/
			break;
		default:
			break;

	}
	}
	return ret;
}



