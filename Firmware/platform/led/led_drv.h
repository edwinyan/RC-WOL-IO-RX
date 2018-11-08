#ifndef _LED_DRV_H_
#define _LED_DRV_H_

typedef enum{
    LED_SRC_START = 0,
    LED_SRC_5V = LED_SRC_START,
    LED_SRC_12V,
    LED_SRC_17V,
    LED_SRC_22V,
    LED_SRC_48V,
    LED_SRC_ALARM1,
    LED_SRC_ALARM2,

    LED_SRC_NUM
}led_src_enum;

#define LED_5V_GPIO			GPIOB
#define LED_5V_PIN			GPIO_Pin_13

#define LED_12V_GPIO		GPIOC
#define LED_12V_PIN			GPIO_Pin_6

#define LED_17V_GPIO		GPIOC
#define LED_17V_PIN			GPIO_Pin_7

#define LED_22V_GPIO		GPIOB
#define LED_22V_PIN			GPIO_Pin_15

#define LED_48V_GPIO		GPIOB
#define LED_48V_PIN			GPIO_Pin_14

#define LED_ALARM1_GPIO		GPIOC
#define LED_ALARM1_PIN		GPIO_Pin_13

#define LED_ALARM2_GPIO		GPIOC
#define LED_ALARM2_PIN		GPIO_Pin_3

void led_drv_init(void);
void led_on(led_src_enum src);
void led_off(led_src_enum src);
void led_toggle(led_src_enum src);
void power_detect(void);


#define LED_5V_ON      	led_on(LED_SRC_5V)
#define LED_12V_ON      led_on(LED_SRC_12V)
#define LED_17V_ON      led_on(LED_SRC_17V)
#define LED_22V_ON      led_on(LED_SRC_22V)
#define LED_48V_ON      led_on(LED_SRC_48V)
#define LED_CTL1_ON     led_on(LED_SRC_ALARM1)
#define LED_CTL2_ON     led_on(LED_SRC_ALARM2)

#define LED_5V_OFF     	led_off(LED_SRC_5V)
#define LED_12V_OFF     led_off(LED_SRC_12V)
#define LED_17V_OFF     led_off(LED_SRC_17V)
#define LED_22V_OFF     led_off(LED_SRC_22V)
#define LED_48V_OFF     led_off(LED_SRC_48V)
#define LED_CTL1_OFF    led_off(LED_SRC_ALARM1)
#define LED_CTL2_OFF    led_off(LED_SRC_ALARM2)

#endif
