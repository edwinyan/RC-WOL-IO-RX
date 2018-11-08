#ifndef _ADC_DRV_H_
#define _ADC_DRV_H_

#define ADC1_IN10_PIN	GPIO_Pin_0
#define ADC1_IN11_PIN	GPIO_Pin_1
#define ADC1_IN12_PIN	GPIO_Pin_2

#define ADC1_IN8_PIN	GPIO_Pin_0
#define ADC1_IN9_PIN	GPIO_Pin_1

#define ADC_NUM_CHANNELS	5

#define ADC_CHANNEL_CAMERA	0		//camera adc channel
#define ADC_CHANNEL_JS_R1	1		//joystick right channel1
#define	ADC_CHANNEL_JS_R2	2		//joystick right channel2
#define ADC_CHANNEL_JS_L1	3		//joystick left channel1
#define	ADC_CHANNEL_JS_L2	4		//joystick left channel2

#define POWER_COMPENSATE		0.5f
#define POWER_COMPENSATE_5V		0.2f
#define POWER_COMPENSATE_48V	2.0f

void  Adc_Init(void);
u16 adc_getvalue(u8 num);



#endif
