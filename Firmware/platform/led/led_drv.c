#include "common.h"
#include "stm32f4xx_gpio.h"
#include "bsp.h"
#include "bsp_os.h"
#include "led_drv.h"
#include "adc_drv.h"
#include "uart_drv.h"
#include "datalink_drv.h"

/*----------------------------------------------------------------------------*/

typedef struct{
    GPIO_TypeDef *port;
    u16         pin;
}led_config_t;

typedef enum{
	ALARM_INIT =0,		//警灯起始状态
	ALARM_STATE1,	//警灯状态1:同时闪烁，频率1s
	ALARM_STATE2,	//警灯状态2:红蓝交替闪烁，频率1s
	ALARM_STATE3,	//警灯状态3:蓝红交替闪烁，频率1s
	ALARM_STATE4,	//警灯状态4:红蓝交替快闪，频率0.5s
	
}AlarmState_e;

STATIC led_config_t led_config_array[LED_SRC_NUM] = {
    //{GPIOC, GPIO_Pin_6},
    {LED_5V_GPIO, LED_5V_PIN},
    {LED_12V_GPIO, LED_12V_PIN},
    {LED_17V_GPIO, LED_17V_PIN},
    {LED_22V_GPIO, LED_22V_PIN}, 
    {LED_48V_GPIO, LED_48V_PIN}, 
    {LED_ALARM1_GPIO, LED_ALARM1_PIN}, 
    {LED_ALARM2_GPIO, LED_ALARM2_PIN}, 
};

#define CONVERT_COEFFICIENT	3.314f*53/4095/2
#define ALARM_TIME			50		// 1min

static float standard[ADC_NUM_CHANNELS] = {48.0f,5.0f,22.0f,12.0f,17.4f}; //标准输入电压值
AlarmState_e alarmstate = ALARM_INIT;
u32 alarm_stamp=0;
extern u8 pack[DATA_LENGTH];

/*----------------------------------------------------------------------------*/
//global functions
void led_drv_init(void)
{
    GPIO_InitTypeDef  gpio_init;
    led_src_enum src;
    
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;

    for(src = LED_SRC_START; src < LED_SRC_NUM; src++)
    {
        gpio_init.GPIO_Pin = led_config_array[src].pin;
        GPIO_Init(led_config_array[src].port, &gpio_init);
    }

}

void led_on(led_src_enum src)
{
    ASSERT_D(src < LED_SRC_NUM);
    GPIO_SetBits(led_config_array[src].port, led_config_array[src].pin);
}

void led_off(led_src_enum src)
{
    ASSERT_D(src < LED_SRC_NUM);
    GPIO_ResetBits(led_config_array[src].port, led_config_array[src].pin);
}

void led_toggle(led_src_enum src)
{
	ASSERT_D(src < LED_SRC_NUM);

	if(GPIO_ReadInputDataBit(led_config_array[src].port, led_config_array[src].pin))
	{
		GPIO_ResetBits(led_config_array[src].port, led_config_array[src].pin);
	}else{
		GPIO_SetBits(led_config_array[src].port, led_config_array[src].pin);
	}
}

void power_detect(void)
{
	//u8 i=0;
	OS_ERR      err;
	float power[ADC_NUM_CHANNELS];	//实际采样到的电压值
	//u16 power[ADC_NUM_CHANNELS];
	
	power[ADC_CHANNEL_48V] = adc_getvalue(ADC_CHANNEL_48V)*CONVERT_COEFFICIENT;
	power[ADC_CHANNEL_5V] = adc_getvalue(ADC_CHANNEL_5V)*CONVERT_COEFFICIENT;
	power[ADC_CHANNEL_22V] = adc_getvalue(ADC_CHANNEL_22V)*CONVERT_COEFFICIENT;
	power[ADC_CHANNEL_12V] = adc_getvalue(ADC_CHANNEL_12V)*CONVERT_COEFFICIENT;
	power[ADC_CHANNEL_17V] = adc_getvalue(ADC_CHANNEL_17V)*CONVERT_COEFFICIENT;
#if 0
	for(i=0;i<5;i++)
	{
		//power[i] = adc_getvalue[i];
		MSG("%f,",power[i]);
	}
	MSG("\r\n");
#endif
	OSTimeDlyHMSM(0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &err);

	if(power[ADC_CHANNEL_5V] > (standard[ADC_CHANNEL_5V] + POWER_COMPENSATE_5V)
		|| power[ADC_CHANNEL_5V] < (standard[ADC_CHANNEL_5V] - POWER_COMPENSATE_5V))
	{
		//电压不正常，状态灯闪烁
		if(power[ADC_CHANNEL_5V] < 1.0f){
			LED_5V_OFF;
		}else{
			led_toggle(LED_SRC_5V);
		}
	}else{
		//电压正常，状态灯常亮
		LED_5V_ON;
	}
	if(power[ADC_CHANNEL_48V] > (standard[ADC_CHANNEL_48V] + POWER_COMPENSATE_48V)
		|| power[ADC_CHANNEL_48V] < (standard[ADC_CHANNEL_48V] - POWER_COMPENSATE_48V))
	{
		//电压不正常，状态灯闪烁
		if(power[ADC_CHANNEL_48V] < 1.0f){
			LED_48V_OFF;
		}else{
			led_toggle(LED_SRC_48V);
		}
	}else{
		//电压正常，状态灯常亮
		LED_48V_ON;
	}
	if(power[ADC_CHANNEL_12V] > (standard[ADC_CHANNEL_12V] + POWER_COMPENSATE)
		|| power[ADC_CHANNEL_12V] < (standard[ADC_CHANNEL_12V] - POWER_COMPENSATE))
	{
		//电压不正常，状态灯闪烁
		if(power[ADC_CHANNEL_12V] < 1.0f){
			LED_12V_OFF;
		}else{
			led_toggle(LED_SRC_12V);
		}
	}else{
		//电压正常，状态灯常亮
		LED_12V_ON;
	}
	if(power[ADC_CHANNEL_17V] > (standard[ADC_CHANNEL_17V] + POWER_COMPENSATE)
		|| power[ADC_CHANNEL_17V] < (standard[ADC_CHANNEL_17V] - POWER_COMPENSATE))
	{
		//电压不正常，状态灯闪烁
		if(power[ADC_CHANNEL_17V] < 1.0f){
			LED_17V_OFF;
		}else{
			led_toggle(LED_SRC_17V);
		}
	}else{
		//电压正常，状态灯常亮
		LED_17V_ON;
	}
	if(power[ADC_CHANNEL_22V] > (standard[ADC_CHANNEL_22V] + POWER_COMPENSATE)
		|| power[ADC_CHANNEL_22V] < (standard[ADC_CHANNEL_22V] - POWER_COMPENSATE))
	{
		//电压不正常，状态灯闪烁
		if(power[ADC_CHANNEL_22V] < 1.0f){
			LED_22V_OFF;
		}else{
			led_toggle(LED_SRC_22V);
		}
	}else{
		//电压正常，状态灯常亮
		LED_22V_ON;
	}
	
	OSTimeDlyHMSM(0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &err);

}

void alarm_control(void)
{
	u8 i;
	
	if(pack[10]%2 == 0)
	{
		alarmstate = ALARM_INIT;
		alarm_stamp= 0;
		led_off(LED_SRC_ALARM1);
		led_off(LED_SRC_ALARM2);
	}
				
	switch(alarmstate){
		case ALARM_INIT:
			if(pack[10]%2 == 1)
			{
				alarm_stamp = ALARM_TIME;
				alarmstate = ALARM_STATE1;
				led_off(LED_SRC_ALARM1);
				led_off(LED_SRC_ALARM2);
				MSG("----enter state 1 ------\r\n");
			}
			break;
		case ALARM_STATE1:
			if(alarm_stamp)
			{
				for(i=0;i<10;i++){
					led_toggle(LED_SRC_ALARM1);
					led_toggle(LED_SRC_ALARM2);
					BSP_OS_TimeDly(500);
					MSG("----alarm state 1 ------\r\n");
				}
			}else{
				alarmstate = ALARM_STATE2;
				alarm_stamp = ALARM_TIME/2;
				led_on(LED_SRC_ALARM1);
				led_off(LED_SRC_ALARM2);
			}
			break;
		case ALARM_STATE2:
			if(alarm_stamp)
			{
				led_toggle(LED_SRC_ALARM1);
				led_toggle(LED_SRC_ALARM2);
				MSG("----alarm state 2 ------\r\n");
			}else{
				alarmstate = ALARM_STATE3;
				alarm_stamp = ALARM_TIME/2;
				led_on(LED_SRC_ALARM2);
				led_off(LED_SRC_ALARM1);
			}
			break;
		case ALARM_STATE3:
			if(alarm_stamp)
			{
				led_toggle(LED_SRC_ALARM1);
				led_toggle(LED_SRC_ALARM2);	
				MSG("----alarm state 3 ------\r\n");
			}else{
				alarmstate = ALARM_STATE4;
				alarm_stamp = ALARM_TIME;
				led_off(LED_SRC_ALARM1);
				led_off(LED_SRC_ALARM2);
			}
			break;
		case ALARM_STATE4:
			if(alarm_stamp)
			{
				MSG("----alarm state 4 ------\r\n");
				for(i=0;i<10;i++)
				{
					led_toggle(LED_SRC_ALARM1);
					BSP_OS_TimeDly(500);
				}
				BSP_OS_TimeDly(2000);
				for(i=0;i<10;i++)
				{
					led_toggle(LED_SRC_ALARM2);
					BSP_OS_TimeDly(500);
				}
			}else{
				//返回状态1
				alarmstate = ALARM_STATE1;
				alarm_stamp = ALARM_TIME;
				led_off(LED_SRC_ALARM1);
				led_off(LED_SRC_ALARM2);
				MSG("----return back to state 1----\r\n");
			}
			break;
		default:
			break;

			
	}
}

