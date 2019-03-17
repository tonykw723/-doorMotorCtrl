/**
  ******************************************************************************
  * @file key.c
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
#include "drvUart.h"
#include "key.h"

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
xt_u8 KEY_learnGet(void) ;
xt_u8 KEY_swapGet(void) ;
/* Public variables ----------------------------------------------------------*/
extern flashData_u flashData;
extern xt_u16 Conversion_Value;
key_t key[KEY_MAX]=
{
	0,KEY_learnGet,0,NONE_PRESS,
	//0,KEY_swapGet,0,NONE_PRESS,
};

xt_u8 KEY_learnGet(void) 
{
	return GPIO_ReadInputPin((GPIO_TypeDef*)KEY_LEARN_GPIO_PORT, (GPIO_Pin_TypeDef)KEY_LEARN_GPIO_PIN);
}
xt_u8 KEY_swapGet(void) 
{
	//return GPIO_ReadInputPin((GPIO_TypeDef*)KEY_SWAP_GPIO_PORT, (GPIO_Pin_TypeDef)KEY_SWAP_GPIO_PIN);
}

void KEY_delRfCode(void)
{
	xt_u8 i;

	flashData.value.rfCodeIdx=0;
	memset(flashData.value.rfCode,0,RF_CODE_MAX*sizeof(xt_u32));
	flashData.value.uidAddr=0x3721;
	flashData.value.password[0]=0;
	flashData.value.password[1]=0;
	flashData.value.password[2]=0;
	flashData.value.password[3]=0;
	flashData.value.password[4]=0;
	flashData.value.password[5]=0;
	flashData.value.password[6]=0x37;
	flashData.value.password[7]=0x21;
	//---把学习的编码写到eeprom中
	for(i=0;i<FLASH_DATA_MAX;i++)
	{		
		IWDG_ReloadCounter();
		FLASH_ProgramByte(FLASH_START_ADDR+i, flashData.buf[i]);
	}	//---指示灯快闪烁，表示学习成功。
	
	LED_flash();
	LED_flash();
	glbPara.learnTimeOut=0;
	
}



void getKeyNoLock(key_t *key)
{
#if 1//HAND_BORAD
	switch(key->step)
	{
		case 0:
			if(key->func()==0)
			{
				key->step=1;
			}
			break;
		case 1:
			if(key->func()==0)//key press
			{
				key->cnt++;
				if(key->cnt>=3)
				{
					key->step=2;
				}
			}
			else//去抖动
			{
				key->step=0;
			}
			break;
		case 2:
			if(key->func()==0)//判长按
			{
				key->cnt++;
				if(key->cnt >= 200)//表示长按200*10ms=2s
				{
					key->step = 4;
					key->val = CONTINUE_PRESS;//删除学习码
					//PRINT_DATA("key continue press");
                                   glbPara.learnTimeOut=3;

				}
			}			
			else//key relase
			{
				if(key->cnt<200)
					key->step=3;
				else
				{
					key->step = 0;////
					key->cnt=0;////
				}
			}
			break;
		case 3:
			if(key->func())//key relase
			{
				key->step = 0;
				key->cnt=0;
				key->val = DOT_PRESS;  //短按学习遥控码
				//PRINT_DATA("key dot press");
                                glbPara.learnTimeOut=LEARN_TIMEOUT;//超时10s来学习。
                                glbPara.flagLock=NO;//屏蔽遥控引导学习，此时用按键学习
			}
			break;
		case 4:
			if(key->func())//key relase
			{
				key->step = 0;
				key->cnt=0;
			}			
			break;
	}
#else
	switch(key->step)
	{
		case 0:
			if(key->func()!=0)
			{
				key->step=1;
			}
			break;
		case 1:
			if(key->func()!=0)//key press
			{
				key->cnt++;
				if(key->cnt>=3)
				{
					key->step=2;
				}
			}
			else//去抖动
			{
				key->step=0;
			}
			break;
		case 2:
			if(key->func()!=0)//判长按
			{
				key->cnt++;
				if(key->cnt >= 200)//表示长按200*10ms=2s
				{
					key->step = 4;
					key->val = CONTINUE_PRESS;//删除学习码
					//PRINT_DATA("key continue press");
                                   glbPara.learnTimeOut=3;

				}
			}			
			else//key relase
			{
				if(key->cnt<200)
					key->step=3;
				else
				{
					key->step = 0;////
					key->cnt=0;////
				}
			}
			break;
		case 3:
			if(key->func()==0)//key relase
			{
				key->step = 0;
				key->cnt=0;
				key->val = DOT_PRESS;  //短按学习遥控码
				//PRINT_DATA("key dot press");
                                glbPara.learnTimeOut=LEARN_TIMEOUT;//超时10s来学习。
                                glbPara.flagLock=NO;//屏蔽遥控引导学习，此时用按键学习
			}
			break;
		case 4:
			if(key->func()==0)//key relase
			{
				key->step = 0;
				key->cnt=0;
			}			
			break;
	}
#endif
}




void GetKeyProcess(void)
{
	xt_u8 i;
	for(i=0;i<KEY_MAX;i++)
	{
		getKeyNoLock(key);
	}
}



void getHandKeyAdcVal(void)
{
	if(ADC1_GetFlagStatus(ADC1_IT_EOC))
	{
		Conversion_Value = ADC1_GetConversionValue();
		ADC1_ClearFlag(ADC1_IT_EOC);
	}
}


void GetHandKey(void)
{
	static xt_u8 cnt_handKey=0;
	static xt_u8 flag_handKey=0;
	getHandKeyAdcVal();

	if(Conversion_Value<200)//release
	{
		flag_handKey=0;
		cnt_handKey=0;
		return;
	}
	else if((Conversion_Value<450)&&(glbPara.flagLock==NO))//383
	{
		if(flag_handKey==0)
		{
			cnt_handKey++;
			if(cnt_handKey>=3)
			{
				flag_handKey=1;
				DoUp(YES);
				return;
			}
		}
	}
	else if((Conversion_Value<600)&&(glbPara.flagLock==NO))//529
	{
		if(flag_handKey==0)
		{
			cnt_handKey++;
			if(cnt_handKey>=3)
			{
				flag_handKey=1;
				DoDown(YES);
				return;
			}
		}
	}
	else if((Conversion_Value<800)&&(glbPara.flagLock==NO))//732
	{
		if(flag_handKey==0)
		{
			cnt_handKey++;
			if(cnt_handKey>=3)
			{
				flag_handKey=1;
				DoRelease();
				return;
			}
		}	
	}
	else //981
	{
		if(flag_handKey==0)
		{
			cnt_handKey++;
			if(cnt_handKey>=3)
			{
				flag_handKey=1;
				RELAY_DOOR_ON();
				glbPara.doorTimeOut=30;
				return;
			}
		}	
	}
}
