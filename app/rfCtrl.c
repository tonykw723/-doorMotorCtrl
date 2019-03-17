/**
  ******************************************************************************
  * @file rfCtrl.c
  * @brief 
  * @author  tony
  * @version  V0.0.1
  * @date    2015-7-5 
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
#include "board.h"
#include "drvUart.h"
#include "rfCtrl.h"
#include "key.h"
#include "ap_action.h"

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
extern rfData_t rfRecv;
extern runningPara_t glbPara;
extern flashData_u flashData;
extern key_t key[KEY_MAX];
void rfCapDisable(void)
{
	//TIM1_Cmd(DISABLE);
	//TIM1->IER &=(0x06^0xff);
	TIM1_DeInit();
}

void rfCapEnable(void)
{
	//TIM1_Cmd(ENABLE);
	//TIM1->IER |=0x06;
	TIM1_Config();
}

void rfCapStart(void)
{
	rfRecv.timeout=RF_TIMEOUT;
	rfRecv.finished=NO;
	//rfCapEnable();
}

xt_s32 rfGetFlashValidCode(xt_u32 rfCode,xt_u8 flag_rf,xt_u8 num)
{
	xt_s32 i;
	xt_u32 codeTemp;
	if(flag_rf==RF_PT2264)
	{
		codeTemp=rfCode>>8;
	}
	else if(flag_rf==RF_EV1527)
	{
		codeTemp=rfCode>>4;
	}
	else
		codeTemp=rfCode;
	//--------找到学习过的编码
	for(i=0;i<num;i++)
	{
		if(flashData.value.rfCode[i]==codeTemp)
		{
			break;
		}
	}
	if(i==RF_CODE_MAX)
	{
		return -1;
	}
	return i;
}


void rfLearnAndStore(xt_u32 rfCode, xt_u8 flag_rf,xt_s32 i)
{
	//xt_u8 buf[4]={0};
	
	
	if(flag_rf==RF_RESET)
	{
		flashData.value.rfCodeIdx=0;
		memset(flashData.value.rfCode,0,RF_CODE_MAX);
	}
	else if(i==-1)//没有存储过此编码
	{
		//更新学习到的编码
		
		if(flashData.value.rfCodeIdx<RF_CODE_MAX)
		{
			if(flag_rf==RF_PT2264)
			{
				flashData.value.rfCode[flashData.value.rfCodeIdx]=rfCode>>8;
				//PRINT_DATA("PT2264 learn");
				
			}
			else if(flag_rf==RF_EV1527)
			{
				flashData.value.rfCode[flashData.value.rfCodeIdx]=rfCode>>4;
				//PRINT_DATA("EV1527 learn");
			}
			else if(flag_rf==RF_COMBINE)
			{
				flashData.value.rfCode[flashData.value.rfCodeIdx]=rfCode;
				//PRINT_DATA("comb learn");
			}		
			
		}
		flashData.value.rfCodeIdx++;
		if(flashData.value.rfCodeIdx>=RF_CODE_MAX)
			flashData.value.rfCodeIdx=2;

		//buf[0]=rfCode>>24;
		//buf[1]=rfCode>>16;
		//buf[2]=rfCode>>8;
		//buf[3]=rfCode;
		//printByteData(buf,4);
	}
	else
	{
		//PRINT_DATA("code have wr");
		goto OUT1;
	}
	
	//---把学习的编码写到eeprom中
	//PRINT_DATA("code to be wr");
	for(i=0;i<FLASH_DATA_MAX;i++)
	{		
		IWDG_ReloadCounter();
		FLASH_ProgramByte(FLASH_START_ADDR+i, flashData.buf[i]);
	}	//---指示灯快闪烁，表示学习成功。
	
OUT1:
	LED_flash();
	LED_flash();
	glbPara.learnTimeOut=0;
	
}

xt_u8 rfAnalysis(void)
{
	xt_s32 i,j;
	xt_u8 temp;
	xt_u32 rfCode[2]={0};
	xt_u8 keyVal,keyVal2;
	rf_e rfType; 
	static xt_u8 cnt_learn=0;
	if((rfRecv.finished==YES)&&(rfRecv.timeout!=0))
	{
		#ifdef OLD_RF
		//rfCapDisable();
		for(j=0;j<1;j++)
		{
			for(i=0;i<24;i++)
			{
				temp=rfRecv.buf[j][2*i+1]/rfRecv.buf[j][2*i];
				if((temp==3)||(temp==4))
				{
					rfCode[j]<<=1;					
				}
				else if(temp==1)
				{
					rfCode[j]<<=1;
                                        rfCode[j]|=1;
				}
				else
				{		
					//rfCapEnable();
					rfRecv.timeout=0;
					return FAILURE_XT;
				}
			}
		}
		rfRecv.timeout=0;
		//rfCapEnable();
		#else
		for(i=0;i<24;i++)
		{
			temp=rfRecv.buf[2*i+1]/rfRecv.buf[2*i];
			if((temp==3)||(temp==4))
			{
				rfCode[0]<<=1;					
			}
			else if(temp==1)
			{
				rfCode[0]<<=1;
                            rfCode[0]|=1;
			}
			else
			{		
				rfRecv.timeout=0;
				return FAILURE_XT;
			}
		}
		for(i=0;i<24;i++)
		{
			temp=rfRecv.buf[50+2*i+1]/rfRecv.buf[50+2*i];
			if((temp==3)||(temp==4))
			{
				rfCode[1]<<=1;					
			}
			else if(temp==1)
			{
				rfCode[1]<<=1;
                            rfCode[1]|=1;
			}
			else
			{		
				rfRecv.timeout=0;
				return FAILURE_XT;
			}
		}
		rfRecv.timeout=0;
		if((rfCode[0])!=(rfCode[1]))
		{
			PRINT_DATA("c Err");
			//rfCode[0]=rfCode[1];
			return FAILURE_XT;
		}
		#endif
		


		keyVal=rfCode[0];//取最低字节
		keyVal2=keyVal&0x0f;
		if((keyVal==PT2264_LOCK)||(keyVal==PT2264_UP)||(keyVal==PT2264_UNLOCK)||(keyVal==PT2264_DOWN))
		{
			//---pt2264
			rfType=RF_PT2264;
			i=rfGetFlashValidCode(rfCode[0],rfType,RF_CODE_MAX);
		}
		else if((keyVal2==0x01)||(keyVal2==0x02)||(keyVal2==0x04)||(keyVal2==0x08))
		{
			//---EA1257
			rfType=RF_EV1527;
			i=rfGetFlashValidCode(rfCode[0],rfType,RF_CODE_MAX);
			keyVal=keyVal2;
		}
		else
		{
			//PRINT_DATA("is combine key");
			rfType=RF_COMBINE;
			i=rfGetFlashValidCode(rfCode[0],rfType,RF_CODE_MAX);			
		}

		if(i==-1)//flash 里面找不到此编码
		{

			if(key[KEY_LEARN].val==DOT_PRESS)//学习rf码
			{
				key[KEY_LEARN].val=NONE_PRESS;
				glbPara.flagLearn=NO;
				rfLearnAndStore(rfCode[0],rfType,i);
				return SUCCESS_XT;
			}
			//PRINT_DATA("learnning code");

			switch(keyVal)
			{
					
				case PT2264_LOCK:
				case EV1527_LOCK:
				case PT2264_UNLOCK:
				case EV1527_UNLOCK:
					
				/*
					if(glbPara.flagLearn==NO)
					{
						cnt_learn++;
						if(cnt_learn>50)
						{
							cnt_learn=0;
							glbPara.learnTimeOut=LEARN_TIMEOUT;
							glbPara.flagLearn=YES;//进入引导式学习，超时时间内LED灯灭，直到学习成功快闪退出。
						}
					}*/
					break;
					
				case PT2264_UP:
				case EV1527_UP:
					if(glbPara.flagLearn==YES)
					{
						key[KEY_LEARN].val=DOT_PRESS;//模拟按键按下
					}
					break;
					
				case PT2264_DOWN:
				case EV1527_DOWN:
					#if 0
					if(glbPara.flagLearn==YES)
					{
						key[KEY_LEARN].val=CONTINUE_PRESS;//模拟按键按下
					}
					#else
					/*
					if(glbPara.flagLearn==YES)
					{
						glbPara.flagLearn=NO;
						if(flashData.value.isUpSwap==NO)
							flashData.value.isUpSwap=YES;
						else
							flashData.value.isUpSwap=NO;
						flashData.value.haveWrited=0x1234;
						writeFlashData();
						LED_flash();
						glbPara.learnTimeOut=0;
					}*/
					#endif
					break;				
				default:
					cnt_learn=0;
					if(glbPara.flagLearn==YES)
					{
						key[KEY_LEARN].val=DOT_PRESS;//模拟按键按下
					}
					break;
			}
			if(glbPara.learnTimeOut==0)//非引导式学习或按键学习
			{
				//PRINT_DATA("uv cd");
				return FAILURE_XT;
			}
			return SUCCESS_XT;
		}
		
		switch(keyVal)
		{
			case PT2264_LOCK:
			case EV1527_LOCK:					
				//PRINT_DATA("RF_LK");
				RELAY_UP_OFF();
				RELAY_DW_OFF();
				RELAY_LOCK_ON();
				RELAY_DOOR_ON();
				glbPara.doorTimeOut=RF_TIMEOUT;
				glbPara.flagLock=YES;

				if((glbPara.flagLearn==NO)&&(i<=1))//(rfGetFlashValidCode(rfCode[0],rfType,2)<0))
				{
					cnt_learn++;
					if(cnt_learn>50)
					{
						cnt_learn=0;
						glbPara.learnTimeOut=LEARN_TIMEOUT;
						glbPara.flagLearn=YES;//进入引导式学习，超时时间内LED灯灭，直到学习成功快闪退出。
					}
				}
				break;
			case PT2264_UP:
			case EV1527_UP:
				if(glbPara.flagLock==NO)
				{						
					DoUp(YES);
				}
				RELAY_DOOR_ON();
				glbPara.doorTimeOut=RF_TIMEOUT;
				cnt_learn=0;
				break;
			case PT2264_UNLOCK:
			case EV1527_UNLOCK:
				PRINT_DATA("RF_UL");
				glbPara.flagLock=NO;
				RELAY_UP_OFF();
				RELAY_DW_OFF();
				RELAY_LOCK_ON();
				RELAY_DOOR_ON();
				 glbPara.doTimeOut=DOT_TIMEOUT;
				glbPara.doorTimeOut=RF_TIMEOUT;
				cnt_learn=0;
				break;
			case PT2264_DOWN:
			case EV1527_DOWN:
				if(glbPara.flagLock==NO)
				{						
					DoDown(YES);
				}
				RELAY_DOOR_ON();
				glbPara.doorTimeOut=RF_TIMEOUT;
				#if 0
				if(glbPara.flagLearn==YES)
				{
					glbPara.learnTimeOut=0;
					key[KEY_LEARN].val=CONTINUE_PRESS;//模拟按键按下
				}		
				#else
				if(glbPara.flagLearn==YES)
				{
					glbPara.flagLearn=NO;
					if(flashData.value.isUpSwap==NO)
						flashData.value.isUpSwap=YES;
					else
						flashData.value.isUpSwap=NO;
					flashData.value.haveWrited=0x1234;
					writeFlashData();
					LED_flash();
					glbPara.learnTimeOut=0;
				}				
				#endif
				cnt_learn=0;
				break;
			default:
				cnt_learn=0;
				if(rfType==RF_COMBINE)
				{
					static xt_u8 oneDoStep=0;
					static xt_u8 flag=0;
					glbPara.flagLock=NO;
					RELAY_DOOR_ON();
					glbPara.doorTimeOut=RF_TIMEOUT;
					switch(oneDoStep)
					{ 
						case 0://up
							if((flashData.value.isContinueAction==NO))
							{
								if(flag==0)
								{
									//DoRelease();
									DoUp(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									glbPara.nxtActionTimeOut=NXT_TIMEOUT;
									oneDoStep=1;
									flag=1;
								}
								else if(glbPara.nxtActionTimeOut==0)
								{
									//DoRelease();
									DoUp(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									glbPara.nxtActionTimeOut=NXT_TIMEOUT;
									oneDoStep=1;
									flag=1;
								}
								else if(glbPara.rfTimeOut==0)
								{
									DoUp(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									glbPara.nxtActionTimeOut=NXT_TIMEOUT;
									oneDoStep=1;
									flag=1;
								}
								break;
							}
							if((flag)&&(glbPara.rfTimeOut>0))
							{
								DoUp(NO);
								glbPara.rfTimeOut=RF_TIMEOUT;
							}
							else if((flag==0))    
							{
								DoUp(NO);
								glbPara.rfTimeOut=RF_TIMEOUT;
								flag=1;
							}
							else if((glbPara.doTimeOut==0))//jump release to down
							{
								DoDown(NO);
								glbPara.rfTimeOut=RF_TIMEOUT;
								oneDoStep=2;
								flag=2;
							}
							else if(glbPara.rfTimeOut==0)//up next action
							{
								DoRelease();
								glbPara.rfTimeOut=RF_TIMEOUT;
								oneDoStep=1;
								flag=1;
							}

							break;
						case 1://up release
							if((flashData.value.isContinueAction==NO))
							{
								if(glbPara.nxtActionTimeOut==0)
								{
									//DoRelease();
									DoDown(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									glbPara.nxtActionTimeOut=NXT_TIMEOUT;
									oneDoStep=3;
									flag=1;
								}
								else if(glbPara.rfTimeOut==0)
								{
									DoRelease();
									//DoDown(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									oneDoStep=2;
									flag=1;
								}
								break;
							}
							if((flag)&&(glbPara.rfTimeOut>0))
							{
								DoRelease();
								glbPara.rfTimeOut=RF_TIMEOUT;
							}
							else if(glbPara.rfTimeOut==0)//up release next action
							{
								DoDown(NO);
								glbPara.rfTimeOut=RF_TIMEOUT;
								oneDoStep=2;
								flag=1;								
							}
							break;
						case 2://down
							if((flashData.value.isContinueAction==NO))
							{
								
								if(glbPara.nxtActionTimeOut==0)
								{
									//DoRelease();
									DoUp(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									glbPara.nxtActionTimeOut=NXT_TIMEOUT;
									oneDoStep=1;
									flag=1;
								}
								else if(glbPara.rfTimeOut==0)
								{
									//DoRelease();
									DoDown(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									oneDoStep=3;
									flag=1;
								}
								break;
							}
							if((flag)&&(glbPara.rfTimeOut>0))
							{
								DoDown(NO);
								glbPara.rfTimeOut=RF_TIMEOUT;
								if(flashData.value.isContinueAction==NO)
								{
									oneDoStep=3;
								}
							}
							else if((glbPara.doTimeOut==0))//jump release to up
							{
								DoUp(NO);
								glbPara.rfTimeOut=RF_TIMEOUT;
								oneDoStep=0;
								flag=1;
							}
							else if(glbPara.rfTimeOut==0)//down next action
							{
								DoRelease();
								glbPara.rfTimeOut=RF_TIMEOUT;
								oneDoStep=3;
								flag=1;
							}
							break;
						case 3://down release
							if((flashData.value.isContinueAction==NO))
							{
								if(glbPara.nxtActionTimeOut==0)
								{
									//DoRelease();
									DoUp(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									glbPara.nxtActionTimeOut=NXT_TIMEOUT;
									oneDoStep=1;
									flag=1;
								}
								else if(glbPara.rfTimeOut==0)
								{
									DoRelease();
									//DoDown(NO);
									glbPara.rfTimeOut=RF_TIMEOUT;
									oneDoStep=0;
									flag=1;
								}
								break;
							}						
							if((flag)&&(glbPara.rfTimeOut>0))
							{
								DoRelease();
								glbPara.rfTimeOut=RF_TIMEOUT;
							}
							else if(glbPara.rfTimeOut==0)//down release next action
							{
								DoUp(NO);
								glbPara.rfTimeOut=RF_TIMEOUT;
								oneDoStep=0;
								flag=1;
							}
							break;
					}
			
					return SUCCESS_XT;
				}
				return FAILURE_XT;
		}
			
		return SUCCESS_XT;
		
		
	}
	return WAIT_XT;
}

void RfCapPrecess(void)
{
	static xt_u8 rStep=0;
	#if 0
	switch(rStep)
	{
		case 0:
			rfCapStart();
			rStep=1;
			break;
		case 1:
			if(rfRecv.timeout==0)
			{
				//rfCapDisable();
				rStep=0;
			}
			if(rfAnalysis()!=WAIT_XT)
			{
				//rfCapDisable();
				rStep=0;
			}
			break;
		case 2:
			break;
	}
	#else
	if(rfRecv.timeout==0)
	{
		rfCapStart();
	}
	if(rfAnalysis()!=WAIT_XT)
	{
		rfCapStart();
	}
	#endif
}
