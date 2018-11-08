#ifndef _DATALINK_DRV_H_
#define _DATALINK_DRV_H_

#define DATA_LENGTH 16
#define PACK_HEAD1	0xAA
#define PACK_HEAD2	0xAA
#define PACK_TAIL1	0x55
#define PACK_TAIL2	0x55

bool datalink_received(void);
u8 datalink_unpack(void);
void gcs_data_send(void);
void get_rssi(void);


#endif
