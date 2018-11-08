#include "common.h"
#include "bsp.h"
#include "bsp_os.h"
#include "uart_drv.h"
#include "datalink_drv.h"
#include "led_drv.h"

u8 pack[DATA_LENGTH]={0};
u8 button=0;
u8 csq_cmd[]="AT*CSQ?\r\n";

extern FIFO_T stFiFo;
extern u32 DutyCycle;

static u8 checkbufferdata(u8 *data,u8 size)
{
	u8 ret;
	u16 sum=0;

	while(size--)
	{
		sum += *(data++);
	}

	ret = sum %256;

	return ret;
}

bool datalink_received(void)
{
	u8 read;
	u8 position;
	u8 temp;
	bool ret=FALSE;
	//OS_ERR      err;
	//u8 rssi=0;
	//u8 data_len=0;
	u8 csq_value[10]="RSSI=";
//	u8 i;

	//FIFO中数据长度要大于一次数据包长度才进行数据的提取
	if(Fifo_DataLen(&stFiFo) >= (DATA_LENGTH+4))
	{
		//MSG("enter data receive function\r\n");
		if(Fifo_Read(&stFiFo,&read) == TRUE && read == PACK_HEAD1)
		{
			if(Fifo_Read(&stFiFo,&read) == TRUE && read == PACK_HEAD2)
			{
				for(position=0;position<DATA_LENGTH;position++)//提取数据段
				{
					Fifo_Read(&stFiFo,&temp);
					pack[position] = temp;	//在发送端，数据经过了翻转处理，此时需要再翻转过来
					//MSG("0x%x,",pack[position]);
				}
				if(Fifo_Read(&stFiFo,&read) == TRUE && read == PACK_TAIL1)
				{
					if(Fifo_Read(&stFiFo,&read) == TRUE && read == PACK_TAIL2) //数据包校验完毕，无错误
					{
						//MSG("we've got a data packet with no error-%d\r\n",OSTimeGet(&err));
						ret = TRUE;
					}
				}
			}
		}else if(read == '+'){
			if(Fifo_Read(&stFiFo,&read) == TRUE && read == 'C'){
				if(Fifo_Read(&stFiFo,&read) == TRUE && read == 'S'){
					if(Fifo_Read(&stFiFo,&read) == TRUE && read == 'Q'){
						if(Fifo_Read(&stFiFo,&read) == TRUE && read == ':'){
							if(Fifo_Read(&stFiFo,&read) == TRUE){
							//	rssi = read - '0';
								csq_value[5] = read;
							}	
							if(Fifo_Read(&stFiFo,&read) == TRUE && read >= '0' && read <= '9'){
							//	rssi = rssi*10 + read - '0';
								csq_value[6] = read;
							}
							//MSG("got rssi value--%d\r\n",rssi);

							uart_drv_data_send(csq_value);
						}
					}
				}
			}
		}
	}
	return ret;
}


u8 datalink_unpack(void)
{
	u8 ret=0;
	u8 zoom_in=0;
	u8 zoom_out=0;
	//u8 led[2]={0};
	//u16 adc[2]={0};
	
	/*正确的拿到了一个数据包的数据*/
	if(pack[DATA_LENGTH-1] == checkbufferdata(pack,15))
	{
		//deviceID = (pack[1]<<24) + (pack[2]<<16) + (pack[3]<<8) + pack[4];
		//adc[0] = (pack[5]<<8) + pack[6];
		//adc[1] = (pack[7]<<8) + pack[8];
		button = pack[9];			
		//led[0] = pack[10];
		//led[1] = pack[11];
		zoom_in = pack[12];
		zoom_out = pack[13];
		//MSG("%x\r\n",deviceID);
		//MSG("%d\r\n",count++);
		//MSG("%d\r\n",adc[1]);
		//MSG("%x\r\n",button);
		//MSG("led1=%d\r\n",pack[10]);
		MSG("%d,%d,%d,%d,%d\r\n",zoom_in,zoom_out,button,pack[10],pack[11]);
		ret=1;
	}
	return ret;
 }

 void gcs_data_send(void)
 {
	 //u8 i;
	 u8 gcs[18]={0};
	 
	 gcs[0] = PACK_HEAD1;
	 gcs[1] = PACK_HEAD2;
	 gcs[2] = pack[5];	 //adc up down high
	 gcs[3] = pack[6];	 //adc up down low
	 gcs[4] = pack[7];	 //adc left right high
	 gcs[5] = pack[8];	 //adc left right low
	 gcs[6] = 0x00; 	 //roll
	 gcs[7] = 0x00; 	 //roll
	 //pack[12] -- zoom in 
	 //pack[13] -- zoom out
	 if(pack[12] == 1 && pack[13] ==0){
		 gcs[8] = 2;	 //zoom in
		 gcs[9] = 4;	 //zoom out
	 }else if(pack[12] == 1 == 0 && pack[13] ==1){
		 gcs[8] = 1;	 //zoom in
		 gcs[9] = 4;	 //zoom out
	 }else{
		 gcs[8] = 0x00; 	 //zoom in
		 gcs[9] = 0x00; 	 //zoom out
	 }
	 gcs[10] = DutyCycle;	 //液位
	 gcs[11] = 0x00;
	 gcs[12] = 0x00;
	 gcs[13] = 0x00;
	 gcs[14] = 0x00; 
	 gcs[15] = checkbufferdata(gcs+2,13);
	 gcs[16] = PACK_TAIL1;
	 gcs[17] = PACK_TAIL2;
 
#if 0
	 for(i=0;i<18;i++)
	 {
		 MSG("%d,",gcs[i]);
	 }
	 MSG("\r\n");
#endif
	 //MSG("DutyCycle = %d\r\n",DutyCycle);
	 uart_drv_433_send(gcs,18);
 }

 void get_rssi(void)
 {
	 uart_drv_data_send(csq_cmd);
 }

