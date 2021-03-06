#include "timer.h"
#include "flash.h"
#include "uart.h"
#include "lcd_display.h"

//----------------time---------------
u8 time_cnt = 0;
u16 time_sec = 0;

void set_time_sec_val(u16 sec)
{
	time_sec = sec;
	time_cnt = 0;
	gm_printf("set time second:%d\r\n",sec);
}


void set_time_sec(void)
{
	time_cnt = 0;
	if(flash_info.timer == TIMER_ON)
		time_sec = 0;
	else if(flash_info.timer == TIMER_0_5H)
		time_sec = 30*60;
	else if(flash_info.timer == TIMER_1H)
		time_sec = 60*60;
	else if(flash_info.timer == TIMER_2H)
		time_sec = 2*60*60;
	else if(flash_info.timer == TIMER_4H)
		time_sec = 4*60*60;
	else if(flash_info.timer == TIMER_8H)
		time_sec = 8*60*60;
	gm_printf("time second:%d \r\n",time_sec);
}

void time0_init(void)
{
	/**********************************TIM0配置初始化**************************************/
	TCON1 = 0x00;						//Tx0定时器时钟为Fosc/12  
	TMOD = 0x00;						//16位重装载定时器/计数器
	//定时10ms
	//反推初值 	= 65536 - ((10/1000) / (1/(Fosc / Timer分频系数)))
	//		   	= 65536 - ((10/1000) / (1/(16000000 / 12)))
	//			= 65536 - 13333
	//			= 0xCBEC
	// TH0 = 0xCB;
	// TL0 = 0xEB;							//T0定时时间10ms  16MHZ
	TH0 = 0xF2;
	TL0 = 0xFB;							//T0定时时间10ms  4MHZ
	TF0 = 0;//清除中断标志
	ET0 = 1;//打开T0中断
	TR0 = 1;//使能T0
	EA = 1;								//打开总中断
}



//10ms
void TIMER0_Rpt(void) interrupt TIMER0_VECTOR
{
	if(flash_info.timer != TIMER_ON && get_device_state() == ON)
	{
		time_cnt++;
		if(time_cnt >=100)
		{
			if(time_sec > 0)
				time_sec--;
			
			else 
			{
				set_device_state(OFF);
				time_sec = 0;
				ht1621_all_clear();
				//定时时间到操作
				gm_printf("time off \r\n");
			}
			gm_printf("time_sec=%d \r\n",time_sec);
			time_cnt = 0;
		}	
	}
}