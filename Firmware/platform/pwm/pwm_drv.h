#ifndef _PWM_DRV_H_
#define _PWM_DRV_H_

//T4CH2 for pwm input capture
#define TIM4_CH2_GPIO	GPIOB
#define TIM4_CH2_PIN	GPIO_Pin_7
#define TIM4_CH2_PINSOURCE	GPIO_PinSource7
//T2CH2 for pwm output
#define TIM2_CH2_GPIO	GPIOB
#define TIM2_CH2_PIN	GPIO_Pin_3
#define TIM2_CH2_PINSOURCE	GPIO_PinSource3

void TIM4_Cap_Init(u32 arr,u16 psc);
void TIM2_PWM_Init(u32 arr,u16 psc);


#endif
