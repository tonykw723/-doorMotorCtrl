/**
  ******************************************************************************
  * @file main.c
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
#include "stm8s.h"
#include "xtsys_type.h"
#include "globalVal.h"
#include "board.h"
#include "drvUart.h"
#include "protocol.h"
#include "ap_action.h"
#include "rfCtrl.h"
#include "key.h"

#if 1
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define DEBUG_ON
/* Private macro -------------------------------------------------------------*/
#ifdef DEBUG_ON
#define	PRINT_DATA printData
#else
#define	PRINT_DATA do{}while(0);
#endif
/* Public variables ---------------------------------------------------------*/
runningPara_t glbPara;
extern xt_u16 Conversion_Value;
extern rfData_t rfRecv;
extern key_t key[KEY_MAX];
extern flashData_u flashData;
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void glbPara_ini(void)
{
    glbPara.recvStartIdx=0;
    glbPara.recvNextIdx=0;

	rfRecv.timeout=RF_TIMEOUT;
	glbPara.learnTimeOut=0;//LEARN_TIMEOUT;
}

/**
  * @brief  Configures the IWDG to generate a Reset if it is not refreshed at the
  *         correct time. 
  * @param  None
  * @retval None
  */

__IO uint32_t LsiFreq = 128772;
static void IWDG_Config(void)
{
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
  
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* IWDG counter clock: LSI/128 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  
  /* Set counter reload value to obtain 250ms IWDG Timeout.
    Counter Reload Value = 250ms/IWDG counter clock period
                         = 250ms / (LSI/128)
                         = 0.25s / (LsiFreq/128)
                         = LsiFreq/(128 * 4)
                         = LsiFreq/512
   */
  IWDG_SetReload((uint8_t)(LsiFreq/1024));
  
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
}

void chkPhase(void)
{
       static xt_u8 chkCnt_up=0;
       static xt_u8 chkFlag_up=0;
       static xt_u8 chkCnt_down=0;
       static xt_u8 chkFlag_down=0;
	   if((CHK_PHASE1_GET()!=0))// high level can not run 
	{
		if(chkFlag_up==0)
		{
			chkCnt_up++;
			if(chkCnt_up>=1)
			{
				chkCnt_up=0;
				chkFlag_up=1;
				RELAY_UP_OFF();
				if(glbPara.flagLock==NO)
					RELAY_LOCK_OFF();
				glbPara.flagPhaseUp=YES;
			}
		}
	}
	else
	{
		chkFlag_up=0;
		glbPara.flagPhaseUp=NO;
	}
	if((CHK_PHASE2_GET()!=0))
	{
		if(chkFlag_down==0)
		{
			chkCnt_down++;
			if(chkCnt_down>=1)
			{
				chkCnt_down=0;
				chkFlag_down=1;
				RELAY_DW_OFF();
				if(glbPara.flagLock==NO)
					RELAY_LOCK_OFF();
				glbPara.flagPhaseDown=YES;
			}
		}
	}
	else
	{
		chkFlag_down=0;
		glbPara.flagPhaseDown=NO;
	}
}


void changePassWord(void)
{
	xt_u8 i;
	if((glbPara.flagPassWord)&&(glbPara.flagBTDisconnect))
	{
		#if defined(BL2_0)
		i=0;
		sendData.outBuf[i++]='A';
		sendData.outBuf[i++]='T';	
		sendData.outBuf[i++]='+';
		sendData.outBuf[i++]='P';	
		sendData.outBuf[i++]='I';
		sendData.outBuf[i++]='N';	
		sendData.outBuf[i++]=glbPara.passWord[0];
		sendData.outBuf[i++]=glbPara.passWord[1];	
		sendData.outBuf[i++]=glbPara.passWord[2];
		sendData.outBuf[i++]=glbPara.passWord[3];			
		sendData.outBuf[i++]=0x0d;
		sendData.outBuf[i++]=0x0a;	
		sendData.outBuf[i++]=0;
		sendData.outLen=i-1;
		glbPara.recvTimeOut=100;
		//uartPackage("12",2);
		PRINT_DATA(sendData.outBuf);
		uartSend(sendData.outBuf,sendData.outLen);
		#elif defined(BLE)
		i=0;
		sendData.outBuf[i++]='A';
		sendData.outBuf[i++]='T';	
		sendData.outBuf[i++]='+';
		sendData.outBuf[i++]='P';	
		sendData.outBuf[i++]='S';
		sendData.outBuf[i++]='W';	
		sendData.outBuf[i++]='D';	
		sendData.outBuf[i++]=glbPara.passWord[0];
		sendData.outBuf[i++]=glbPara.passWord[1];	
		sendData.outBuf[i++]=glbPara.passWord[2];
		sendData.outBuf[i++]=glbPara.passWord[3];			
		sendData.outBuf[i++]=0x0d;
		sendData.outBuf[i++]=0x0a;	
		sendData.outBuf[i++]=0;
		sendData.outLen=i-1;
		glbPara.recvTimeOut=100;
		//uartPackage("12",2);
		PRINT_DATA(sendData.outBuf);
		uartSend(sendData.outBuf,sendData.outLen);
		#endif
	}
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    xt_u8 flag=0,i;

  /* Borad configuration -----------------------------------------*/
    Board_Init(); 
    glbPara_ini();
    IWDG_Config();
    PRINT_DATA("into main");
    while (1)
    {
       RfCapPrecess();
  	if(uartRecv()==SUCCESS_XT)
  	{
		if(proParse(&recvData)==SUCCESS_XT)
		{
			Application();
		}
  	}
	if((glbPara.msCnt%10<3))//10ms
	{
		//IWDG_ReloadCounter();
		if(flag==0)
		{
			flag=1;
			IWDG_ReloadCounter();
			GetKeyProcess();		
			//HandKeyapplication();
			//GetHandKey();
			if(rfRecv.timeout>0)
				rfRecv.timeout--;
			if(glbPara.rfTimeOut>0)
				glbPara.rfTimeOut--;
			if(glbPara.doorTimeOut>0)
				glbPara.doorTimeOut--;
			//if(flashData.value.isContinueAction)//联动才需要检测相位
			//	chkPhase();
			if((glbPara.doTimeOut==0))
			{
				RELAY_UP_OFF();
				RELAY_DW_OFF();
				if(glbPara.flagLock==NO)
					RELAY_LOCK_OFF();
			}
			else
			{
				glbPara.doTimeOut--;
			}
		}

	}
	else 
	{
		flag=0;
	}
	if(glbPara.doorTimeOut==0)
		RELAY_DOOR_OFF();
	if(glbPara.flagOneSec)
	{	
		glbPara.flagOneSec=NO;
		if(glbPara.nxtActionTimeOut>0)
		{
			glbPara.nxtActionTimeOut--;
		}
		if(glbPara.learnTimeOut>0)
		{
			glbPara.learnTimeOut--;
			LED_RUN_H();
		}
		else
		{
			if(key[KEY_LEARN].val==CONTINUE_PRESS)//长按学习健，删除学习码
			{
				key[KEY_LEARN].val=NONE_PRESS;
				KEY_delRfCode();
			}
			key[KEY_LEARN].val=NONE_PRESS;
			glbPara.flagLearn=NO;
			glbPara.flagDel=NO;

			LED_RUN_TOGGLE();
		}
		PRINT_DATA("tick");
		//changePassWord();
		#if 0
		#if defined(BL2_0)
		i=0;
		sendData.outBuf[i++]='A';
		sendData.outBuf[i++]='T';	
		sendData.outBuf[i++]='+';
		sendData.outBuf[i++]='P';	
		sendData.outBuf[i++]='I';
		sendData.outBuf[i++]='N';	
		#elif defined(BLE)
		i=0;
		sendData.outBuf[i++]='A';
		sendData.outBuf[i++]='T';	
		sendData.outBuf[i++]='+';
		sendData.outBuf[i++]='A';	
		sendData.outBuf[i++]='D';
		sendData.outBuf[i++]='S';	
		sendData.outBuf[i++]='S';	
		#endif
		sendData.outBuf[i++]=0x0d;
		sendData.outBuf[i++]=0x0a;	
		sendData.outBuf[i++]=0;
		sendData.outLen=i-1;
		glbPara.recvTimeOut=100;
		//uartPackage("12",2);
		PRINT_DATA(sendData.outBuf);
		uartSend(sendData.outBuf,sendData.outLen);
		#endif
		
	}
    }
}
#else

#endif

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
