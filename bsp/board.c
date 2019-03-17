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
#include "protocol.h"
#include "key.h"
#include "board.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM2_PERIOD       124
//-DEBUG UART
#define DEBUG_GPIO_PORT  (GPIOB)
#define DEBUG_GPIO_PIN  (GPIO_PIN_5)
#define DEBUG_TIM4_PERIOD       103//12M--77 16M-103
#define DEBUG_GPIO_TX_H() DEBUG_GPIO_PORT->ODR |= (uint8_t)DEBUG_GPIO_PIN//GPIO_WriteHigh(DEBUG_GPIO_PORT, (GPIO_Pin_TypeDef)DEBUG_GPIO_PIN)
#define DEBUG_GPIO_TX_L() DEBUG_GPIO_PORT->ODR &= (uint8_t)(~DEBUG_GPIO_PIN)//GPIO_WriteLow(DEBUG_GPIO_PORT, (GPIO_Pin_TypeDef)DEBUG_GPIO_PIN)



/* Private macro -------------------------------------------------------------*/
#define DEBUG_ON
#ifdef DEBUG_ON
#define	PRINT_DATA printData
#else
#define	PRINT_DATA do{}while(0);
#endif
/* Private variables ---------------------------------------------------------*/
//uint16_t ICValue1 =0, ICValue2 =0;
flashData_u flashData;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
 #define USE_HSI
 //#define USE_HSE

 void delayxx(xt_u32 time)
 {
 	xt_u32 count;
	for(;time>0;time--)
	{
		for(count=0;count<100;count++)
			nop();
	}
 }
 
static void CLK_Config(void)
{
#if 1
    //uint32_t freq=0;
    /* Initialization of the clock */
    CLK_DeInit();
    /* Clock divider to HSI/1 */
    	
#if defined(USE_HSI)
/*
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
    */
    CLK->CKDIVR=0;
    CLK->CMSR=0XE1;
    CLK->SWCR=2;
    //CLK_CCOConfig(CLK_OUTPUT_CPU);
    //CLK_CCOCmd(ENABLE);	
#elif defined(USE_HSE)
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
    //GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
    //CLK_CCOConfig(CLK_OUTPUT_LSI);
    //CLK_CCOCmd(ENABLE);
    //freq=CLK_GetClockFreq();
#endif
#else
	uint32_t freq=0;
	uint16_t DownCounter = CLK_TIMEOUT;
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_CPUDIV);
    CLK->CKDIVR |= (uint8_t)((uint8_t)CLK_PRESCALER_CPUDIV1 & (uint8_t)CLK_CKDIVR_CPUDIV);
    /* Enables Clock switch */
    CLK->SWCR |= CLK_SWCR_SWEN;
    CLK->SWCR &= (uint8_t)(~CLK_SWCR_SWIEN);
    CLK->SWR = (uint8_t)CLK_SOURCE_HSE;//CLK_SOURCE_HSI;//CLK_SOURCE_HSE;
    /* Wait until the target clock source is ready */
    while((((CLK->SWCR & CLK_SWCR_SWBSY) != 0 )&& (DownCounter != 0)))
    {
      DownCounter--;
    }
    CLK->ICKR &= (uint8_t)(~CLK_ICKR_HSIEN);//(~CLK_ICKR_HSIEN);
	freq=CLK_GetClockFreq();
#endif
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void UART_Config(void)
{
  /* Deinitializes the UART1 and UART3 peripheral */
    UART1_DeInit();
    /* UART1 and UART3 configuration -------------------------------------------------*/
    /* UART1 and UART3 configured as follow:
          - BaudRate = 9600 baud  
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Receive and transmit enabled
          - UART1 Clock disabled
     */
    /* Configure the UART1 */
#if 0
	flashData.value.baudRate=96;
	UART1_Init((uint32_t)(flashData.value.baudRate*100), UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
                UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    
    /* Enable UART1 Transmit interrupt*/

    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
#else
    UART1->CR1=0x00;
    UART1->CR2=0x2c;
    UART1->CR3=0x00;
    #if defined(BL2_0)
    //--9600
    //12M
    #if defined(USE_12M)
    UART1->BRR2=0X02;//02-9600 
    UART1->BRR1=0X4e;//4E-9600 
    //16M
    #elif defined(USE_16M)
    UART1->BRR2=0X03;//02-9600 
    UART1->BRR1=0X68;//4E-9600  
    #endif
    #elif defined(BLE)
    //--115200
    	#if defined(USE_12M)
    UART1->BRR2=0X08;//08-115200
    UART1->BRR1=0X06;//06-115200
    	#elif defined(USE_16M)
    UART1->BRR2=0X05;//05-115200
    UART1->BRR1=0X14;//14-115200
    	#endif
    #endif
    //UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
#endif
     
}

/**
  * @brief  for one ms ,for run heart beat
  * @param  None
  * @retval None
  */
static void TIM2_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   //---16M
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (65535 + 1) / 125000 = 524.3 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 
    //---12M
      clock used is 12 MHz / 128 = 93750 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (65536 + 1) / 93750 =  ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 93750 =  ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 93750 - 1) = 93 */

#if 0
  /* Time base configuration */
  TIM2_TimeBaseInit(TIM2_PRESCALER_128, TIM2_PERIOD);
  /* Clear TIM2 update flag */
  TIM2_ClearFlag(TIM2_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  /* Enable TIM2 */
  TIM2_Cmd(ENABLE);
  #else
  
   #if defined(USE_12M)
  TIM2->PSCR=0X07;
  TIM2->ARRH=0X00;
  TIM2->ARRL=0X5D;
  TIM2->SR1=0X00;
  TIM2->SR2=0X00;
  TIM2->IER=0X01;
  TIM2->CR1=0X01;
  #elif defined(USE_16M)
  TIM2->PSCR=0X07;
  TIM2->ARRH=0X00;
  TIM2->ARRL=0X7c;
  TIM2->SR1=0X00;
  TIM2->SR2=0X00;
  TIM2->IER=0X01;
  TIM2->CR1=0X01;  
  #endif
  #endif
}



/**
  * @brief  
  * @param  None
  * @retval None
  */
static void debugUart_Tim4Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   //---16M
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 
    //---12M
      clock used is 12 MHz / 16 = 750000 Hz
  - With 125 000 Hz we can generate time base:
  - In this example we need to generate a time base equal to 8us
   so TIM4_PERIOD = (1/9600 * 750000 - 1) = 77 

    //---16M
      clock used is 16 MHz / 16 = 1000000 Hz
  - With 125 000 Hz we can generate time base:
  - In this example we need to generate a time base equal to 8us
   so TIM4_PERIOD = (1/9600 * 1000000 - 1) = 103 */
#if 0
  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_16, DEBUG_TIM4_PERIOD);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  //TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
 #else
  #if defined(USE_12M)
 	TIM4->PSCR=0X04;
 	TIM4->ARR=0X4D;
 	TIM4->CR1=0X01;
 #elif defined(USE_16M)
 	TIM4->PSCR=0X04;
 	TIM4->ARR=0X67;
 	TIM4->CR1=0X01;
 #endif
 #endif
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
void DebugUart_SendData8(xt_u8 ch)
{
	xt_u8 i=0;
	//disableInterrupts();
	IWDG_ReloadCounter();
	while(TIM4_GetFlagStatus(TIM4_FLAG_UPDATE)==RESET);
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	DEBUG_GPIO_TX_L();		//start bit 1
	while(TIM4_GetFlagStatus(TIM4_FLAG_UPDATE)==RESET);
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	for(i = 0; i < 8; i++)		//data bit 8
	{
		if ((ch & (0x1<<i)) != 0)
			DEBUG_GPIO_TX_H();		
		else
			DEBUG_GPIO_TX_L();		

		while(TIM4_GetFlagStatus(TIM4_FLAG_UPDATE)==RESET);
		TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	}

	DEBUG_GPIO_TX_H();	
	while(TIM4_GetFlagStatus(TIM4_FLAG_UPDATE)==RESET);
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	IWDG_ReloadCounter();
	// enableInterrupts(); 
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void debugUart_Config(void)
{
#if 0
    GPIO_Init(DEBUG_GPIO_PORT, (GPIO_Pin_TypeDef)DEBUG_GPIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
#else
    DEBUG_GPIO_PORT->CR2 &= (uint8_t)(~(DEBUG_GPIO_PIN));
    DEBUG_GPIO_PORT->ODR |= (uint8_t)DEBUG_GPIO_PIN;
    DEBUG_GPIO_PORT->DDR |= (uint8_t)DEBUG_GPIO_PIN;
    DEBUG_GPIO_PORT->CR1 |= (uint8_t)DEBUG_GPIO_PIN;
    DEBUG_GPIO_PORT->CR2 |= (uint8_t)DEBUG_GPIO_PIN;
#endif
    DEBUG_GPIO_TX_H();	
    debugUart_Tim4Config(); 
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void LED_Config(void)
{
#if 1
    GPIO_Init((GPIO_TypeDef*)LED_RUN_GPIO_PORT, (GPIO_Pin_TypeDef)LED_RUN_GPIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
#else
    LED_RUN_GPIO_PORT->CR2 &= (uint8_t)(~(LED_RUN_GPIO_PIN));
    LED_RUN_GPIO_PORT->ODR |= (uint8_t)LED_RUN_GPIO_PIN;
    LED_RUN_GPIO_PORT->DDR |= (uint8_t)LED_RUN_GPIO_PIN;
    LED_RUN_GPIO_PORT->CR1 |= (uint8_t)LED_RUN_GPIO_PIN;
    LED_RUN_GPIO_PORT->CR2 |= (uint8_t)LED_RUN_GPIO_PIN;
#endif
}

void LED_flash(void)
{
    LED_RUN_TOGGLE();
    delayxx(3000);
    LED_RUN_TOGGLE();
    delayxx(3000);
    LED_RUN_TOGGLE();	
    delayxx(3000);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void relay_Config(void)
{
	#if defined(HAND_BORAD)||defined(SECOND_VER)
       GPIO_Init(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
	//GPIO_Init(RELAY_DOOR_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DOOR_GPIO_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
	#elif defined(FIRST_VER)
       GPIO_Init(RELAY_UP_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_UP_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(RELAY_DW_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_DW_GPIO_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
	//GPIO_Init(RELAY_LOCK_GPIO_PORT, (GPIO_Pin_TypeDef)RELAY_LOCK_GPIO_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
	#endif

	RELAY_UP_OFF();
	RELAY_DW_OFF();
	RELAY_LOCK_OFF();
	RELAY_DOOR_OFF();
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void key_Config(void)
{
   GPIO_Init((GPIO_TypeDef*)KEY_LEARN_GPIO_PORT,(GPIO_Pin_TypeDef)KEY_LEARN_GPIO_PIN,(GPIO_Mode_TypeDef)GPIO_MODE_IN_PU_NO_IT);
   //GPIO_Init(KEY_SWAP_GPIO_PORT, (GPIO_Pin_TypeDef)KEY_SWAP_GPIO_PIN, GPIO_MODE_IN_PU_NO_IT);

}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void chkPhase_Config(void)
{
   GPIO_Init((GPIO_TypeDef*)CHK_PHASE1_GPIO_PORT,(GPIO_Pin_TypeDef)CHK_PHASE1_GPIO_PIN,(GPIO_Mode_TypeDef)GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init((GPIO_TypeDef*)CHK_PHASE2_GPIO_PORT,(GPIO_Pin_TypeDef)CHK_PHASE2_GPIO_PIN,(GPIO_Mode_TypeDef)GPIO_MODE_IN_PU_NO_IT);

}

/**
  * @brief  Configure TIM1 to to capture the internal clock source (LSI)
  * @param  None
  * @retval None
  */
void TIM1_Config(void)
{
  //GPIO_Init(GPIOC , GPIO_PIN_6 , GPIO_MODE_IN_PU_IT);  //TIM1_CH1   PC6端口

#if 0
//---CC1
  TIM1_ICInit( TIM1_CHANNEL_1, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI,
               TIM1_ICPSC_DIV8, 0x0);

  TIM1_CCxCmd( TIM1_CHANNEL_1 , ENABLE);


  /* Clear CC1 Flag*/
  TIM1_ClearFlag(TIM1_FLAG_CC1);
  TIM1_ITConfig( TIM1_IT_CC1 , ENABLE);
  /* Enable TIM1 */
//---CC3
  TIM1_ICInit( TIM1_CHANNEL_3, TIM1_ICPOLARITY_FALLING, TIM1_ICSELECTION_DIRECTTI,
               TIM1_ICPSC_DIV8, 0x0);

  TIM1_CCxCmd( TIM1_CHANNEL_3 , ENABLE);


  /* Clear CC1 Flag*/
  TIM1_ClearFlag(TIM1_FLAG_CC3);
  TIM1_ITConfig( TIM1_IT_CC3 , ENABLE);
  /* Enable TIM1 */
  TIM1_Cmd(ENABLE);
  #if 0
  /* wait a capture on CC1 */
  while((TIM1->SR1 & TIM1_FLAG_CC1) != TIM1_FLAG_CC1);
  /* Get CCR1 value*/
  ICValue1 = TIM1_GetCapture1();
  TIM1_ClearFlag(TIM1_FLAG_CC1);
  
  /* wait a capture on cc1 */
  while((TIM1->SR1 & TIM1_FLAG_CC1) != TIM1_FLAG_CC1);
  /* Get CCR1 value*/
  ICValue2 = TIM1_GetCapture1();
  TIM1_ClearFlag(TIM1_FLAG_CC1);
  #endif
#else
TIM1_DeInit();
 TIM1->PSCRH = 0;
 TIM1->PSCRL = 3; 
//时基单元
//时钟触发控制器
TIM1->SMCR |=0X64;//选源触发源ti2fp2 和触发方式复位
//捕获比较
TIM1->CCMR1 |=0x02;// ch1 到ti2fp1
TIM1->CCMR2 |=0x01;// ch1 到ti2fp1
TIM1->CCER1 |= 0x13;//使能输入捕获ti2fp1 ti2fp2 ti2fp1 下降升沿ti2fp2 上升沿

//中断
TIM1->IER |=0x06;//使能中断ch1 ch2
TIM1_Cmd(ENABLE);
/*
TIM1_DeInit();        
TIM1->CCER1 &= 0xee;//禁止输入捕获1，2     
TIM1->CCMR1 |=0x01;// ch1到ti1fp1       
//TIM1->CCMR1 |=0x0C;
TIM1->CCER1&= 0xec;//ch1 上升沿触发    
TIM1->CCMR2 |=0x02;// ch2选择 ti1fp2上    
//TIM1->CCMR2 |=0x0C;
TIM1->CCER1|= 0x20;//ch1 下降沿触发 ic2   
TIM1->SMCR |=0X54;//选源触发源和触发方式 复位   
TIM1->IER |=0x06;//使能中断ch1 ch2  选择了TS=101，则 触发信号为上升沿 触发的动作是复位 CC1被选择为输入
TIM1->CCER1 |= 0x11;//使能输入捕获1，2    
TIM1_Cmd(ENABLE);*/
#endif

}


/**
  * @brief  Configure the FLASH for block programming
  * @param  None
  * @retval None
  */

void FLASH_Config(void)
{



 /* Define flash programming Time*/
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);

  FLASH_Unlock(FLASH_MEMTYPE_PROG);
  /* Wait until Flash Program area unlocked flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_PUL) == RESET)
  {}

  /* Unlock flash data eeprom memory */
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  /* Wait until Data EEPROM area unlocked flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
  {}
}

void readFlashData(void)
{
	xt_u16 i;
	xt_u8 len=sizeof(flashData.value);
	for(i=0;i<len;i++)
	{
		IWDG_ReloadCounter();
		flashData.buf[i]= FLASH_ReadByte(FLASH_START_ADDR+i);
	}
}

void writeFlashData(void)
{
	xt_u16 i;
	xt_u8 len=sizeof(flashData.value);
	for(i=0;i<len;i++)
	{		
		IWDG_ReloadCounter();
		disableInterrupts();
		FLASH_ProgramByte(FLASH_START_ADDR+i, flashData.buf[i]);
		IWDG_ReloadCounter();
		enableInterrupts();
	}	
}

void readCfgData(void)
{
	readFlashData();
	//printByteData(flashData.buf, len);
	if(flashData.value.haveWrited==0x1234)
	{
		//PRINT_DATA("flash have writed");
		//if(flashData.value.baudRate==96)
			//PRINT_DATA("baudrate:9600");
		//else if(flashData.value.baudRate==1152)
			//PRINT_DATA("baudrate:115200");
	}
	else
	{
		//PRINT_DATA("flash empty and set default");
		flashData.value.haveWrited=0x1234;
		flashData.value.uidAddr=0x3721;
		flashData.value.password[0]=0;
		flashData.value.password[1]=0;
		flashData.value.password[2]=0;
		flashData.value.password[3]=0;
		flashData.value.password[4]=0;
		flashData.value.password[5]=0;
		flashData.value.password[6]=0x37;
		flashData.value.password[7]=0x21;
		flashData.value.rfCode[0]=0;//出厂固定码，给生产用。
		//flashData.value.baudRate=1152;//96;
		//FLASH_ProgramBlock(BLOCK_OPERATION, FLASH_MEMTYPE_DATA, FLASH_PROGRAMMODE_STANDARD, flashData.buf);
		writeFlashData();
	}
	flashData.value.isContinueAction=NO;
	IWDG_ReloadCounter();
}

void chk_resetSource(void)
{
  /* Check if the system has resumed from IWDG reset */
  if (RST_GetFlagStatus(RST_FLAG_IWDGF) != RESET)
  {
  	PRINT_DATA("reset from iwdg");
	/* Clear IWDGF Flag */
       RST_ClearFlag(RST_FLAG_IWDGF);
  }
}


/**
  * @brief  Configure ADC2 Continuous Conversion with End Of Conversion interrupt 
  *         enabled .
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
  /*  Init GPIO for ADC2 */
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);

  #if 0
  /* De-Init ADC peripheral*/
  ADC1_DeInit();

  /* Init ADC2 peripheral */
  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_4, ADC1_PRESSEL_FCPU_D2, \
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL4,\
            DISABLE);

  /* Enable EOC interrupt */
  //ADC1_ITConfig(ADC1_IT_EOCIE,ENABLE);

  /* Enable general interrupts */  
  //enableInterrupts();
  
  /*Start Conversion */
  ADC1_StartConversion();
  #else
   //ADC1_DeInit();

  ADC1->CR2=0X08;  
  ADC1->CR1=0X02;
  ADC1->CSR=0X04;
  ADC1->TDRL=0X10; 
  ADC1->CR1|=0X01;
  ADC1->CR1 |= ADC1_CR1_ADON;
  //ADC1_StartConversion();
  #endif
}


/**
  * @brief 
  * @param  None
  * @retval None
  */
void Board_Init(void)
{
  /* Clock configuration -----------------------------------------*/
  CLK_Config();   
  /* debug UART configuration -----------------------------------------*/
  debugUart_Config();   
  //chk_resetSource();
  /* FLASH Configuration ------------------------------------------*/
  FLASH_Config();
  readCfgData();
  /* UART configuration -----------------------------------------*/
  UART_Config();
  /* TIM2 configuration -----------------------------------------*/
  TIM2_Config();
   /* TIM1 configuration for RF analysis-----------------------------------------*/
  TIM1_Config();
  //-io config
  LED_Config();
  relay_Config();
  key_Config();
  //chkPhase_Config();
  //-adc config
  //ADC_Config();
  /* enable interrupts ------------------------------------------*/
  enableInterrupts();   
  LED_flash();
}
