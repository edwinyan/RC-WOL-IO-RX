#include "common.h"
#include "bsp.h"
#include "bsp_os.h"
#include "led_drv.h"
//#include "tc_common.h"
#include "uart_drv.h"
#include "datalink_drv.h"
#include "pwm_drv.h"
#include "wt588d_drv.h"

OS_MUTEX	TX_MUTEX;		//uart tx mutex
OS_MUTEX	RX_MUTEX;		//uart rx mutex

OS_MUTEX	FIFO_MUTEX;

FIFO_T stFiFo;

extern u32 DutyCycle;
extern u32 pwmFlag;
extern u32 alarm_stamp;
extern u8 button;
/*----------------------------------------------------------------------------*/
//macro and variables
#define  APP_CFG_TASK_START_STK_SIZE                    256u
#define  APP_CFG_TASK_START_PRIO                        5u

#define  APP_RX_TASK_STK_SIZE                    256u
#define  APP_RX_TASK_PRIO                        4u

#define  APP_UART_TASK_STK_SIZE                  256u
#define  APP_UART_TASK_PRIO                      3u

#define  APP_ALARM_TASK_STK_SIZE                  256u
#define  APP_ALARM_TASK_PRIO                      6u

#define  APP_RSSI_TASK_STK_SIZE                    256u
#define  APP_RSSI_TASK_PRIO                        15u

static  OS_TCB   app_task_start_tcb;
static  CPU_STK  app_task_start_stk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   app_rx_task_tcb;
static  CPU_STK  app_rx_task_stk[APP_RX_TASK_STK_SIZE];

static  OS_TCB   app_uart_task_tcb;
static  CPU_STK  app_uart_task_stk[APP_UART_TASK_STK_SIZE];

static  OS_TCB   app_alarm_task_tcb;
static  CPU_STK  app_alarm_task_stk[APP_ALARM_TASK_STK_SIZE];

static  OS_TCB   app_rssi_task_tcb;
static  CPU_STK  app_rssi_task_stk[APP_RSSI_TASK_STK_SIZE];

/*----------------------------------------------------------------------------*/
//local function
STATIC void app_wol_io_rx_task(void *p_arg)
{
	OS_ERR      err;

	(void)p_arg;
	
	MSG("Creating Application Tasks: %d\r\n",__FPU_USED);

	while (DEF_TRUE) 
    {   
    	if( datalink_received())
	  	{
			if(datalink_unpack() ==0)
			{
				MSG("packet data error detected\r\n");
			}
			//receive_done =FALSE;
		}
		OSTimeDlyHMSM(0, 0, 0, 30, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

STATIC void app_wol_io_uart_task(void *p_arg)
{
	OS_ERR      err;
	u8 data=0;

	(void)p_arg;
	
	//MSG("Creating Application Tasks: app_wol_io_uart_task\r\n");

	Fifo_Init(&stFiFo);

	while (DEF_TRUE) 
    {   
		//MSG("-----------uart-----------@%d\r\n",OSTimeGet(&err));
        #if 1
		if(1 == uart_drv_data_recv(&data,1))
		{
			Fifo_Write(&stFiFo,data);
		//MSG("%c,",data);
		}
		//MSG("\r\n");
		#endif
		OSTimeDlyHMSM(0, 0, 0, 3, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

STATIC void app_wol_io_alarm_task(void *p_arg)
{
	OS_ERR      err;

	(void)p_arg;
	
	while (DEF_TRUE) 
    {   
		//MSG("-----------uart-----------@%d\r\n",OSTimeGet(&err));
		//警灯控制
		alarm_control();
		
		OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

STATIC void app_wol_io_rssi_task(void *p_arg)
{
	OS_ERR      err;
	(void)p_arg;
	
	 while (DEF_TRUE) 
    {       
    	get_rssi();
		//MSG("rssi\r\n");
        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

STATIC void app_task_create(void)
{

	OS_ERR      err;
#if 1	
    OSTaskCreate((OS_TCB       *)&app_rx_task_tcb,              
                 (CPU_CHAR     *)"App wol io rx Task",
                 (OS_TASK_PTR   )app_wol_io_rx_task, 
                 (void         *)0,
                 (OS_PRIO       )APP_RX_TASK_PRIO,
                 (CPU_STK      *)&app_rx_task_stk[0],
                 (CPU_STK_SIZE  )app_rx_task_stk[APP_RX_TASK_STK_SIZE / 10],
                 (CPU_STK_SIZE  )APP_RX_TASK_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
#endif
	OSTaskCreate((OS_TCB	   *)&app_uart_task_tcb,			  
				 (CPU_CHAR	   *)"App wol io uart Task",
				 (OS_TASK_PTR	)app_wol_io_uart_task, 
				 (void		   *)0,
				 (OS_PRIO		)APP_UART_TASK_PRIO,
				 (CPU_STK	   *)&app_uart_task_stk[0],
				 (CPU_STK_SIZE	)app_uart_task_stk[APP_UART_TASK_STK_SIZE / 10],
				 (CPU_STK_SIZE	)APP_UART_TASK_STK_SIZE,
				 (OS_MSG_QTY	)0,
				 (OS_TICK		)0,
				 (void		   *)0,
				 (OS_OPT		)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR	   *)&err);
	OSTaskCreate((OS_TCB       *)&app_alarm_task_tcb,              
	             (CPU_CHAR     *)"App wol io alarm Task",
	             (OS_TASK_PTR   )app_wol_io_alarm_task, 
	             (void         *)0,
	             (OS_PRIO       )APP_ALARM_TASK_PRIO,
	             (CPU_STK      *)&app_alarm_task_stk[0],
	             (CPU_STK_SIZE  )app_alarm_task_stk[APP_ALARM_TASK_STK_SIZE / 10],
	             (CPU_STK_SIZE  )APP_ALARM_TASK_STK_SIZE,
	             (OS_MSG_QTY    )0,
	             (OS_TICK       )0,
	             (void         *)0,
	             (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
	             (OS_ERR       *)&err);
	OSTaskCreate((OS_TCB	   *)&app_rssi_task_tcb, 			 
				 (CPU_CHAR	   *)"App wol io rssi detect Task",
				 (OS_TASK_PTR	)app_wol_io_rssi_task, 
				 (void		   *)0,
				 (OS_PRIO		)APP_RSSI_TASK_PRIO,
				 (CPU_STK	   *)&app_rssi_task_stk[0],
				 (CPU_STK_SIZE	)app_rssi_task_stk[APP_RSSI_TASK_STK_SIZE / 10],
				 (CPU_STK_SIZE	)APP_RSSI_TASK_STK_SIZE,
				 (OS_MSG_QTY	)0,
				 (OS_TICK		)0,
				 (void		   *)0,
				 (OS_OPT		)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR	   *)&err);

}

STATIC void app_task_start(void *p_arg)
{
    OS_ERR      err;
	//u32 i =0;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */
    
    BSP_Tick_Init();                                            /* Start Tick Initialization                            */
    
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
    Math_Init();                                                /* Initialize Mathematical Module                       */

    BSP_Peripheral_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    //MSG("Creating Application Tasks: %d\r\n",__FPU_USED);
	OSMutexCreate((OS_MUTEX*	)&FIFO_MUTEX,
				  (CPU_CHAR*	)"UART FIFO_MUTEX",
				  (OS_ERR*		)&err);
	OSMutexCreate((OS_MUTEX*	)&TX_MUTEX,
				  (CPU_CHAR*	)"UART TX_MUTEX",
				  (OS_ERR*		)&err);
	OSMutexCreate((OS_MUTEX*	)&RX_MUTEX,
				  (CPU_CHAR*	)"UART RX_MUTEX",
				  (OS_ERR*		)&err);

    app_task_create();                                            

    while (DEF_TRUE) 
    {   
        //tc_run_all();
        #if 0
			if(pwmFlag==1){
				//MSG("duty=%d\r\n",DutyCycle);
				//MSG("Frequency=%d\r\n",Frequency);
				pwmFlag=0;
			}else{
				DutyCycle =0;			
			}
			//MSG("duty=%d\r\n",DutyCycle);
		#endif
		//舵机控制信号输出
		pwm_output();
		//语音模块输出
		wt588d_handler(button);
		//电压监测
		power_detect();
		//发送数据给GCS
		gcs_data_send();

		//设置系统100ms的递减
		if(alarm_stamp > 0)
		{
			alarm_stamp --;
		}
        //MSG("----------------loop-----------------\r\n");
        //LED_G_ON;
		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);
		//LED_G_OFF;
		//OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}


/*----------------------------------------------------------------------------*/
int main(void)
{
    OS_ERR err;

    //BSP_IntDisAll();
    OSInit(&err);

    OSTaskCreate((OS_TCB       *)&app_task_start_tcb,              
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )app_task_start, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&app_task_start_stk[0],
                 (CPU_STK_SIZE  )app_task_start_stk[APP_CFG_TASK_START_STK_SIZE / 10],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              
    
    (void)&err;
    
    return 0;
}

