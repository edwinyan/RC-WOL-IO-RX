#ifndef _BUTTON_DRV_H_
#define _BUTTON_DRV_H_

typedef enum{
    BUTTON_SRC_START = 0,	//电机启动
    BUTTON_SRC_IDLE,    	//电机待机
    BUTTON_SRC_RUN,			//电机运行
	
	BUTTON_SRC_NUM
}button_src_enum;

#define BUTTION_START_GPIO	GPIOC
#define BUTTION_START_PIN	GPIO_Pin_8

#define BUTTION_IDLE_GPIO	GPIOC
#define BUTTION_IDLE_PIN	GPIO_Pin_9

#define BUTTION_RUN_GPIO	GPIOA
#define BUTTION_RUN_PIN		GPIO_Pin_8


#define PWM_START	1000
#define PWM_IDLE	1500
#define	PWM_RUN		1950


void button_drv_init(void);
u8 button_value_get(button_src_enum src);

#endif
