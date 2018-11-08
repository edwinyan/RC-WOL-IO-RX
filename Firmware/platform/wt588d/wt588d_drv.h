#ifndef _WT588D_DRV_H_
#define _WT588D_DRV_H_

#define WT588D_DATA_GPIO	GPIOB
#define WT588D_DATA_PIN		GPIO_Pin_12

#define WT588D_DATA_ON		GPIO_SetBits(WT588D_DATA_GPIO,WT588D_DATA_PIN)
#define WT588D_DATA_OFF		GPIO_ResetBits(WT588D_DATA_GPIO,WT588D_DATA_PIN)


#define WT588D_DATA_ALARM			0x00			//·Ë¾¯
#define WT588D_DATA_BEEP			0x10			//ßÙßÙ
#define WT588D_DATA_DUDU			0x20			//à½à½
//#define WT588D_DATA_WHISTLE			0x90			//ÆûµÑ
//#define WT588D_DATA_BACKUP			0xb0			//±¸ÓÃ

#define WT588D_VOLUME_1				0xE0
#define WT588D_VOLUME_2				0xE1
#define WT588D_VOLUME_3				0xE2
#define WT588D_VOLUME_4				0xE3
#define WT588D_VOLUME_5				0xE4
#define WT588D_VOLUME_6				0xE5
#define WT588D_VOLUME_7				0xE6
#define WT588D_VOLUME_8				0xE7

#define WT588D_LOOP 				0xF2
#define WT588D_STOP					0xFE

#define FIRE_ALARM			0x01
#define TERROR_ALARM		0x02
#define AMBULANCE_ALARM		0x04

void wt588d_init(void);
//void wt588d_send_oneline(u8 addr);
void wt588d_handler(u8 input);

#endif
