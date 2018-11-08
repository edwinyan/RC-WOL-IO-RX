#include "common.h"
#include "stm32f4xx_gpio.h"
#include "mc8618_drv.h"
#include "bsp.h"
#include "bsp_os.h"
#include "uart_drv.h"
#include "button_drv.h"
#include "led_drv.h"

/*
mc8618 data packet

20bytes

byte0 - byte3   : deviceID
byte4 - byte5   : data head
byte6 - byte9   : 摇杆 adc
byte10          : buttons value
byte11 - byte14 : backup
byte15 - byte16 : crc16 checksum
byte17 - byte18 : data tail

完整的数据包: AT+ZIPSEND=0,20 0x0D deviceID head adc button backup crc16 tail
*/
/*----------------------------------------------------------------------------*/
void delayms(u16 ms)
{
	BSP_OS_TimeDly(ms*10);
}


void mc8618_gpio_init(void)
{
	GPIO_InitTypeDef  gpio_init;

	gpio_init.GPIO_Pin 	 = MC8618_DATA_POWER_PIN;
	gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_DOWN;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(MC8618_DATA_POWER_GPIO, &gpio_init);
	
	gpio_init.GPIO_Pin 	 =  MC8618_VOICE_POWER_PIN;
	gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_DOWN;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(MC8618_VOICE_POWER_GPIO, &gpio_init);

	delayms(1000);   //delay 1s
	MC8618_DATA_POWER_OFF;
	MC8618_VOICE_POWER_OFF;
	delayms(3000);   //delay 3s
	MC8618_DATA_POWER_ON;
	MC8618_VOICE_POWER_ON;
}



void mc8618_init(void)
{
	mc8618_gpio_init();
	//get_deviceID();
}

