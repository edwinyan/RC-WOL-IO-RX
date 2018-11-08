#include "common.h"
#include "stm32f4xx_gpio.h"
#include "wt588d_drv.h"
#include "bsp.h"
#include "bsp_os.h"


/*----------------------------------------------------------------------------*/

void wt588d_init(void)
{
	GPIO_InitTypeDef  gpio_init;

	gpio_init.GPIO_Pin 	 = WT588D_DATA_PIN;
	gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(WT588D_DATA_GPIO, &gpio_init);

	WT588D_DATA_ON;
}



static void wt588d_send_oneline(u8 addr)
{
	u8 i;
	
	WT588D_DATA_OFF;
	BSP_OS_TimeDly(50);   //timedly 时基100us，延时5ms

	for(i=0;i<8;i++)
	{
		WT588D_DATA_ON;
		if(addr&1)
		{
			BSP_OS_TimeDly(6);	//高电平比低电平为600us:200us，表示发送数据1
			WT588D_DATA_OFF;
			BSP_OS_TimeDly(2);
		}else{
			BSP_OS_TimeDly(2);
			WT588D_DATA_OFF;
			BSP_OS_TimeDly(6);	//高电平比低电平为200us:600us，表示发送数据0
		}
		addr >>=1;
	}
	WT588D_DATA_ON;
	//MSG("set wt588d voice output addr%d\r\n",addr);
}

void wt588d_handler(u8 input)
{
	static u8 fire_enable=0;
	static u8 dudu_enable=0;
	static u8 beep_enable=0;
	
	if(input&FIRE_ALARM)	//未调用过火警语音
		{
			if(fire_enable==0)
			{
				wt588d_send_oneline(WT588D_DATA_ALARM);
				BSP_OS_TimeDly(300);
				wt588d_send_oneline(WT588D_LOOP);
				fire_enable =1;
				dudu_enable =0;
				beep_enable =0;
			}
		}else if(input&TERROR_ALARM){
			if(dudu_enable ==0)
			{
				wt588d_send_oneline(WT588D_DATA_DUDU);
				dudu_enable = 1;
				beep_enable = 0;
				fire_enable = 0;
			}
		}else if(input&AMBULANCE_ALARM){			
			if(beep_enable ==0){
				wt588d_send_oneline(WT588D_DATA_BEEP);
				beep_enable = 1;
				dudu_enable = 0;
				fire_enable = 0;
			}
		}else{
			if(fire_enable)
			{
				wt588d_send_oneline(WT588D_DATA_ALARM);
				BSP_OS_TimeDly(20);
				wt588d_send_oneline(WT588D_STOP);
				fire_enable =0;
			}else if(dudu_enable)
			{
				wt588d_send_oneline(WT588D_DATA_DUDU);
				dudu_enable = 0;
			}else if(beep_enable ==1){
				wt588d_send_oneline(WT588D_DATA_BEEP);
				beep_enable = 0;
			}
		}

}


