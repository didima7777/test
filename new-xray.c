#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "handler.h"
#include "button.h"
#include "dac.h"
#include "spi.h"
#include "lcd/driver_ssd1303.h"
#include "Menu.h"
#include "uart.h"
#include "eeprom.h"
#include  "work_func.h"
#include  "adc.h"
#include  "message.h"

void Init_port(void)
{
 DDRF|=1<<PF3;
 
 DDRG|=1<<PG3;
 PORTG|=1<<PG3;

 DDRD|=1<<7; 

}



int main(void)
{
 flag_ready_fire=0;

 
 Init_dac();
 Init_port();
 LCD_Init ();
 UART0Init();
 Init_adc();
 Init_message(&main_message_list);

 write_dac(0,2);
 restore_param();
 restore_param_cal();

 PORTF|=1<<PF3; //On Power


 Init_handler(&main_timer0);
 add_handler(&main_timer0,5,check_button,1,-1);
 add_handler(&main_timer0,200,dac1,2,-1); 							// set
 add_handler(&main_timer0,95,dac2,3,0); 							// fire  Подобрать задержку //95 - 100ms
 add_handler(&main_timer0,999,warming_down,5,-1);   				// warming_down
 add_handler(&main_timer0,2,warming_up_on_start,6,dac_h1);			// Прогрев	
 add_handler(&main_timer0,45,update_adc,7,-1);						// Измерение U от 1 до 110


 set_handler_anykey(&main_button,press_down_anykey);
 add_button(&main_button,0,&PINE,4,menu,NULL,NULL);		//	menu
 add_button(&main_button,1,&PINE,7,set_2,set_2,NULL);	//	change U anode
 add_button(&main_button,2,&PINE,6,set_1,set_1,NULL);	//	change time exposure
 add_button(&main_button,3,&PINE,5,set,NULL,NULL); 		//	set
 add_button(&main_button,4,&PIND,0,fire,NULL,NULL); 	//	fire

 Init_Timer0();
 MenuInit();

 sei();

 while(1)
 {
	idle();
 };

 return 0;
}
