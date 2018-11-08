#ifndef _ADC_DRV_H_
#define _ADC_DRV_H_

#define ADC1_IN10_PIN	GPIO_Pin_0
#define ADC1_IN11_PIN	GPIO_Pin_1
#define ADC1_IN12_PIN	GPIO_Pin_2

#define ADC1_IN8_PIN	GPIO_Pin_0
#define ADC1_IN9_PIN	GPIO_Pin_1

#define ADC_NUM_CHANNELS	5

#define ADC_CHANNEL_48V		0		
#define ADC_CHANNEL_5V		1		
#define	ADC_CHANNEL_22V		2		
#define ADC_CHANNEL_12V		3		
#define	ADC_CHANNEL_17V		4


#define POWER_COMPENSATE		0.5f
#define POWER_COMPENSATE_5V		0.2f
#define POWER_COMPENSATE_48V	2.0f

void  Adc_Init(void);
u16 adc_getvalue(u8 num);



#endif
