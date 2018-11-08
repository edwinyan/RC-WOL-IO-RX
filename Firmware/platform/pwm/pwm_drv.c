#include "common.h"
#include "stm32f4xx_gpio.h"
#include "pwm_drv.h"
#include "bsp.h"
#include "bsp_os.h"

/*----------------------------------------------------------------------------*/
void TIM2_PWM_Init(u32 arr,u16 psc)
{		 					 	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	GPIO_PinAFConfig(TIM2_CH2_GPIO,TIM2_CH2_PINSOURCE,GPIO_AF_TIM2);
	
	GPIO_InitStructure.GPIO_Pin = TIM2_CH2_PIN;           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       
	GPIO_Init(TIM2_CH2_GPIO,&GPIO_InitStructure);             
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_Period=arr; 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); 

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
 
  	TIM_ARRPreloadConfig(TIM2,ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);
 
										  
}  

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获一次
//[6]:0,还没有捕获到低电平;1,已经捕获到低电平
//[5:0]:捕获低电平后溢出的次数(对于16位定时器来说，1us计数加1)
#if 1

u32 IC2Value=0;
u32 Frequency=0;
u32 DutyCycle=0;
u32 pwmFlag=0;


void TIM4_IRQHandler(void)
{ 	
	OSIntEnter();
	//if(pwmFlag){
		if(TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)
		{
			IC2Value = TIM_GetCapture2(TIM4);
			DutyCycle =IC2Value - TIM_GetCapture1(TIM4);

			Frequency = 1000000/IC2Value;
			if(DutyCycle < 1100){
				DutyCycle = 1100;
			}else if(DutyCycle > 1900){
				DutyCycle = 1900;
			}
			DutyCycle = (DutyCycle - 1100)/8;
		}
		pwmFlag=1;
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
	//}
	OSIntExit();
}



//输入捕获初始化
void TIM4_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	GPIO_InitStructure.GPIO_Pin = TIM4_CH2_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(TIM4_CH2_GPIO,&GPIO_InitStructure);

	GPIO_PinAFConfig(TIM4_CH2_GPIO,TIM4_CH2_PINSOURCE,GPIO_AF_TIM4);
  
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
#if 1
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	BSP_IntVectSet(BSP_INT_ID_TIM4,TIM4_IRQHandler);
	BSP_IntEn(BSP_INT_ID_TIM4);
#endif	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInit(TIM4,&TIM_ICInitStructure);
	TIM_PWMIConfig(TIM4, &TIM_ICInitStructure);

	TIM_SelectInputTrigger(TIM4,TIM_TS_TI2FP2);

	TIM_SelectSlaveMode(TIM4,TIM_SlaveMode_Reset);
	
	TIM_SelectMasterSlaveMode(TIM4,TIM_MasterSlaveMode_Enable);
	TIM_ITConfig(TIM4,TIM_IT_CC2,ENABLE);
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_CC2);

	TIM_Cmd(TIM4,ENABLE);

}


#endif













/*----------------------------------------------------------------------------*/



