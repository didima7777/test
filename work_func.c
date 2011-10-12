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


volatile unsigned char 		flag_pre_work=0;
volatile unsigned char 		flag_work=1;

volatile unsigned short 	dac_h1;
volatile unsigned short 	dac_h2;

volatile unsigned short 	cal_U;
volatile unsigned short 	cal_I;


volatile  long int  		anode_voltage;
volatile  long int			anode_current;

volatile  short  			rms_anode_voltage;
volatile  short	 			rms_anode_current;


volatile unsigned short 	count_sample;


volatile unsigned short 	num_rate=0;  //	В режиме редактирования
volatile unsigned short 	num_group=0; // В режиме редактирования

volatile unsigned short 	del_t;		 // Данные времени из памяти
volatile unsigned short 	u_anode;	 // Рабочий ток анода

volatile unsigned short 	new_del_t;
volatile unsigned short 	new_u_anode;

volatile unsigned short		test_u_anode=400;
volatile unsigned short		cal_u_anode=500;
volatile unsigned short		test_del_t=0;
volatile unsigned short		test_off_on=0;

const char  				*text_group[3]={"Муж","Жен","Реб"};
const char  				*text_rate[4]={"1-3","4-5","6-7","8"};


char						*passwd="AAAAAAA";
volatile unsigned char 		index_pos=0;
volatile unsigned char 		index_char=0;

unsigned short				index_group=0; // В рабочем режиме
unsigned short				index_mode=0;  // В рабочем режиме

unsigned char				start_flag=0;
unsigned char				fire_flag=0;
unsigned char				wait_flag=0;
unsigned char				warm_flag=0;
unsigned char				warm_flag_down=0;
unsigned char				flag_pree_any_key=0;
unsigned char				flag_ready_fire=0;
unsigned char				flag_ext_menu=0;
unsigned char				flag_start_adc=0;

unsigned char				flag_password_access=0;

int 						count_cmd=0;

char 						str[69];
char 						str1[69];
char 						str2[69];
char 						rec_char;

short						v_array[250];
unsigned char				v_index_l=0;
unsigned char				v_index_h=6;

extern void update_menu(void);

void dac1(void) //Подготовка рабочего режима
{
static int i=0;//dac_h1=2000;
static unsigned char j=0;

  if (start_flag==1)
  {
	i+=(dac_h2-dac_h1)/10;

	if ((i+dac_h1)>dac_h2) 
	 {
	   start_flag=0;			  	
	   i=0; j=0;
	   flag_ready_fire=1;
	   return ;
	 };
	write_dac(dac_h1+i,1); 
	if (i%((dac_h2-dac_h1)/10)==0)
	{	   
	   j+=10;
	   sprintf(str,"подготовка %d %%    ",j);
	   OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
	};
  };
};

void dac2(void) //Работа рентгена
{
static int i=0;
	if (i>=2) flag_start_adc=1;
	if (i++>=del_t+2) //Засветку(ток) уронить вниз в конце del_t тоже что  для u_anode
	{	  
	  enable_all(&main_timer0);
	  disable_handler(&main_timer0,7);
	  write_dac(0,2); // возвращение к прогретому состоянию
	  write_dac(dac_h1,1);
	  fire_flag=0;
	  flag_ready_fire=0;
	  flag_start_adc=0;
	  sprintf(str,"выстрел  окончен      ");
	  OLED_PutText(0,48, FONT_1X,FONT_fine,1,str);  //str to LCD

	  rms_anode_voltage=anode_voltage/count_sample;
	  rms_anode_voltage=(short)(rms_anode_voltage*(cal_U/100.0)); //0.71
	  rms_anode_current=anode_current/count_sample;
	  rms_anode_current=(short)(rms_anode_current*(cal_I/100.0)); //2.00

	  sprintf(str,"U=%4d.%dкВ I=%dмкА",rms_anode_voltage/10,rms_anode_voltage%10,rms_anode_current/10);
	  OLED_PutText(0,56, FONT_1X,FONT_fine,1,str); //str to LCD	  	  	
	  i=0;		  	
	  return ;
	};
	write_dac((int)(4096.0*((double)u_anode/1000.0)),2); //Установка высокого уровня u_anode перерасчет в fire
};

void warming_up_on_start(void) //предварительнй прогрев
{
static int i=0;

	if (warm_flag_down==1) warm_flag_down=0;

	write_dac(i++,1);	
	if (i%(dac_h1/100)==0 && i/(dac_h1/100)<=100) 
	 {
		sprintf(str,"Нагрев %d%%",i/(dac_h1/100));
		OLED_PutText(24,20, FONT_1X,FONT_fine,0,str); //str to LCD
	 };
	if (i>(dac_h1-1)) 
	{
	  OLED_PutText(24,20, FONT_1X,FONT_fine,0,"Нагрев вкл!!!"); //str to LCD
	  LCD_clear(0);
	  GO_MENU_FUNC(UPDATE);//update_menu();
	  i=0;
	  warm_flag=1;
	  PORTG&=~(1<<PG3);
	  return ;
	};
};

void warming_up(void)
{
	static int i=0;

	if (wait_flag==1 && warm_flag==0)
	{
		write_dac(i++,1);
		if (i>dac_h1) 
		{
			i=0;
			wait_flag=0;
			warm_flag=1;
		};
	};
};


void warming_down(void) //Выключение предварительного прогрева
{
	static short count=0;

	if (warm_flag==1)
	 {
	 	if (flag_pree_any_key==1)
		 {
			count=0;
			flag_pree_any_key=0;
			return;
		 };
		if (count++>500) //подоюрать до 3 минут
		{
			write_dac(0,1);
			warm_flag=0;
			count=0;
			warm_flag_down=1;
			flag_ready_fire=0;
			LCD_clear(0);
			OLED_PutText(0,0, FONT_1X,FONT_fine,1,"Нагрев выкл"); //str to LCD
			return ;
		};
	 };
};

void check_button(void)
{	
	check_buttons(&main_button);
};


void update_adc(void)
{
	message_send(&main_message_list,(void*)&handler_adc);
};

void handler_adc(void)
{	
 short tmp;

	if (fire_flag==1 && flag_start_adc==1)
	{
	 anode_current+=read_adc(0);
	 tmp=read_adc(1);
	 v_array[count_sample]=tmp;
	 anode_voltage+=tmp;
	 count_sample++;
	};

	if (test_off_on==1)
	{
	  rms_anode_current=read_adc(0)*(cal_I/100.0);
	  rms_anode_voltage=(short)(read_adc(1)*(cal_U/100.0));
	  sprintf(str,"U=%d.%01dкВ I=%dмкА ",rms_anode_voltage/10,rms_anode_voltage%10,rms_anode_current/10);
	  cli();	
	   OLED_PutText(0,56, FONT_1X,FONT_fine,1,str);	  //str to LCD 	  	
	  sei();
	};
};








void set_1(void)
{
 Menu_Item* p;

 p=SIBLING;
 p=((Menu_Item*)pgm_read_word(&p->Next));

 GO_MENU_FUNC((FuncPtr*)pgm_read_word(&p->SelectFunc1));
};

void set_2(void)
{
 Menu_Item* p;

 p=SIBLING;

 GO_MENU_FUNC((FuncPtr*)pgm_read_word(&p->SelectFunc1));
};


void menu(void)
{
  GO_MENU_FUNC(ENTERFUNC);
};

void menu_next_1(void)
{
//Убран пароль
	SET_MENU(NEXT);
	LCD_clear(0);
	GO_MENU_FUNC(UPDATE);//update_menu();

/*
 if (flag_password_access==1)
 {
	SET_MENU(NEXT);
	LCD_clear(0);
	GO_MENU_FUNC(UPDATE);//update_menu();
 } else
 {
 	SET_MENU(PREVIOUS);
	LCD_clear(0);
	GO_MENU_FUNC(UPDATE);//update_menu();
 };
 */
};


void menu_next_2(void)
{
		SET_MENU(PARENT);
		LCD_clear(0);
		GO_MENU_FUNC(UPDATE);//update_menu();
};

void goto_menu1(void)
{
 Menu_Item* p;
 p=SIBLING;
 SET_MENU(p);
 LCD_clear(0);
 GO_MENU_FUNC(UPDATE);//update_menu();
};

void goto_menu2(void)
{
 Menu_Item* p;

 new_u_anode=read_mode_U(num_group,num_rate);
 new_del_t=read_mode_T(num_group,num_rate);

 p=SIBLING;
 p=((Menu_Item*)pgm_read_word(&p->Next));

 SET_MENU(p);
 LCD_clear(0);
 GO_MENU_FUNC(UPDATE);//update_menu();

};




void set_mode(void)
{
	if (warm_flag==1)
	{
		wait_flag=1;
		index_mode++;
		index_mode%=4;
		GO_MENU_FUNC(UPDATE);
	};
};



void set_group(void)
{
	if (warm_flag==1)
	{
		wait_flag=1;
		index_group++;
		index_group%=3;
		GO_MENU_FUNC(UPDATE);
	};
}

void set_dac_h1(void)
{
		dac_h1+=10;
		dac_h1%=4000;
		GO_MENU_FUNC(UPDATE);//update_menu();
};

void set_dac_h2(void)
{
  		dac_h2+=10;
		dac_h2%=4000;
		GO_MENU_FUNC(UPDATE);//update_menu();
};

void set(void)
{
	GO_MENU_FUNC(SELECTFUNC2);
};

void set1(void)
{
	if (warm_flag==1)
	{	
		start_flag=1;
		dac1();
		sprintf(str,"установка             ");
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
	};
};


void set2(void)
{
  store_param();
  sprintf(str,"сохранение параметров накала");
  OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
};

void set3(void)
{
	
	write_mode_U(num_group,num_rate,new_u_anode);
	write_mode_T(num_group,num_rate,new_del_t);

	sprintf(str,"сохранение параметров");
	OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
};

void set4(void)
{	
	if (test_off_on==0)
	{
	 write_dac((int)(4096.0*((double)test_u_anode/1000.0)),2);
	 sprintf(str,"выстрел               ");
     OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD	  	
	 enable_handler(&main_timer0,7);
	 test_off_on=1;
	} else
	{
	  disable_handler(&main_timer0,7);
	  write_dac(0,2); 		// нет анодного тока
	  write_dac(dac_h1,1); // возвращение к прогретому состоянию
	  flag_ready_fire=0;
	  sprintf(str,"выстрел  окончен      ");
	  OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
	  test_off_on=0;	  	
	};
};


void fire(void)
{	
	if (flag_ready_fire==1)
	{
		u_anode=read_mode_U(index_group,index_mode);
		del_t=read_mode_T(index_group,index_mode);
		count_sample=0;
	 	anode_current=0;
	 	anode_voltage=0;

		fire_flag=1;
		sprintf(str,"выстрел             ");
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
		disable_all_only(&main_timer0,3);
		enable_handler(&main_timer0,7);
		reset_timer_handler(&main_timer0,3,del_t+3);
	} 
};



void press_down_anykey(void)
{
	
	flag_pree_any_key=1;
	reset_timer_handler(&main_timer0,5,-1);
	if (warm_flag_down==1) 
	{
		LCD_clear(0);
		reset_timer_handler(&main_timer0,6,dac_h1);	//Прогрев
	};
};





void update_menu(void)
{
 Menu_Item* p;
 unsigned char i;
 short *data;
 
 OLED_PutText_flash(0,0, FONT_1X,FONT_fine,1,MENU_TEXT); //flash str to LCD
 DrawLine(128,64-10,0,64-10,1);
 p=SIBLING;

 i=12;
 while(p!=(void*)&NULL_ENTRY)
 {
 	
	OLED_PutText_flash(0,i, FONT_1X,FONT_fine,1,(unsigned char*)(p->Text));  //flash str to LCD

	data=(short*)pgm_read_word(&p->data);

	if (data!=(void*)&NULL_ENTRY)
	{
		sprintf(str,"%4d",*data);
		OLED_PutText(100,i, FONT_1X,FONT_fine,1,str);  //str to LCD
	};

	p=((Menu_Item*)pgm_read_word(&p->Next));
	i+=28;
 };
};

void update_menu_edit(void)
{
  Menu_Item* p;
 unsigned char i;
 short *data;
 short tmp_data;

 OLED_PutText_flash(0,0, FONT_1X,FONT_fine,1,MENU_TEXT); //flsh str to LCD
 DrawLine(128,64-10,0,64-10,1);
 p=SIBLING;

 	i=12;
 	
	OLED_PutText_flash(0,i, FONT_1X,FONT_fine,1,(unsigned char*)(p->Text)); //flash str to LCD
	data=(short*)pgm_read_word(&p->data);
	if (data!=(void*)&NULL_ENTRY)
	{		
		tmp_data=*data;
		sprintf(str,"%4d.%1d%%",tmp_data/10,tmp_data%10);
		OLED_PutText(80,i, FONT_1X,FONT_fine,1,str);  //str to LCD
	};
	
	p=((Menu_Item*)pgm_read_word(&p->Next));
	i+=28;
	OLED_PutText_flash(0,i, FONT_1X,FONT_fine,1,(unsigned char*)(p->Text));  //flash str to LCD
	data=(short*)pgm_read_word(&p->data);
	if (data!=(void*)&NULL_ENTRY)
	{
		
		tmp_data=*data*5;
		sprintf(str,"%3d.%02dс ",tmp_data/100,tmp_data%100);
		OLED_PutText(80,i, FONT_1X,FONT_fine,1,str); //str to LCD
	};


	sprintf(str,"%s %s",text_group[num_group],text_rate[num_rate]);
	OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
};



void update_menu_test(void)
{
  Menu_Item* p;
 unsigned char i;
 short *data;
 short tmp_data;

 OLED_PutText_flash(0,0, FONT_1X,FONT_fine,1,MENU_TEXT); //flash str to LCD
 DrawLine(128,64-10,0,64-10,1);
 p=SIBLING;

 	i=12;
 	
	OLED_PutText_flash(0,i, FONT_1X,FONT_fine,1,(unsigned char*)(p->Text)); //flash str to LCD
	data=(short*)pgm_read_word(&p->data);
	if (data!=(void*)&NULL_ENTRY)
	{		
		tmp_data=*data;
		sprintf(str,"%4d.%1d%%",tmp_data/10,tmp_data%10);
		OLED_PutText(80,i, FONT_1X,FONT_fine,1,str); //str to LCD
	};
	
	p=((Menu_Item*)pgm_read_word(&p->Next));
	i+=28;
	OLED_PutText_flash(0,i, FONT_1X,FONT_fine,1,(unsigned char*)(p->Text)); //flash str to LCD
};




void update_menu_for_main(void)
{
 Menu_Item* p;
 unsigned char i;
 short *data;
 
 OLED_PutText_flash(0,0, FONT_1X,FONT_fine,1,MENU_TEXT);//flash str to LCD
 DrawLine(128,64-10,0,64-10,1);
 p=SIBLING;

 i=12;
 	
	OLED_PutText_flash(0,i, FONT_1X,FONT_fine,1,(unsigned char*)(p->Text)); //flash str to LCD
	data=(short*)pgm_read_word(&p->data);
	if (data!=(void*)&NULL_ENTRY)
	{
		sprintf(str,"%4s",text_rate[*data]);
		OLED_PutText(100,i, FONT_1X,FONT_fine,1,str);  //str to LCD
	};
	p=((Menu_Item*)pgm_read_word(&p->Next));
	i+=28;
	OLED_PutText_flash(0,i, FONT_1X,FONT_fine,1,(unsigned char*)(p->Text)); //flash str to LCD
	data=(short*)pgm_read_word(&p->data);
	if (data!=(void*)&NULL_ENTRY)
	{
		
		sprintf(str,"%4s",text_group[*data]);
		OLED_PutText(100,i, FONT_1X,FONT_fine,1,str); //str to LCD
	};

};



void set_num_group(void)
{
		num_group+=1;
		num_group%=3;
		GO_MENU_FUNC(UPDATE);//update_menu();
		store_param();

};

void set_num_rate(void)
{
		num_rate+=1;
		num_rate%=4;
		GO_MENU_FUNC(UPDATE);//update_menu();
		store_param();
};







void set_new_u_anode(void)
{

		new_u_anode+=2;
		if (new_u_anode>=950) new_u_anode=400;
		if (new_u_anode<400) new_u_anode=400;
		GO_MENU_FUNC(UPDATE);//update_menu();

};

void set_new_t_exposure(void)
{
		new_del_t++;
		new_del_t%=110;
		GO_MENU_FUNC(UPDATE);// update_menu();
};

void set_test_u_anode(void)
{
		if (test_off_on==1)
		{
		 test_u_anode+=2;
		 if (test_u_anode>=950) test_u_anode=400;
		 if (test_u_anode<400)  test_u_anode=400;
		 write_dac((int)(4096.0*((double)test_u_anode/1000.0)),2);
		 GO_MENU_FUNC(UPDATE);
		} else
		{
		 test_u_anode+=2;
		 if (test_u_anode>=950) test_u_anode=400;
		 if (test_u_anode<400)  test_u_anode=400;
		 GO_MENU_FUNC(UPDATE);
		};
};


void set_test_t_exposure(void)
{
	if (warm_flag==1)
	{	
		start_flag=1;
		dac1();
		sprintf(str,"установка             ");
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
	};
};


void update_passwd(void)
{
 Menu_Item* p;
 unsigned char i;
 char *data;
 
 OLED_PutText_flash(0,0, FONT_1X,FONT_fine,1,MENU_TEXT); //flash str to LCD
 DrawLine(128,64-10,0,64-10,1);
 sprintf(str,"Password:%s",passwd);
 OLED_PutText(20,24, FONT_1X,FONT_fine,1,str); //str to LCD

};

void set_pos()
{
		index_pos++;
		index_pos%=7;
		index_char=passwd[index_pos]-'A';
		GO_MENU_FUNC(UPDATE);// update_menu();
		OLED_PutChar_new(passwd[index_pos],121-(74+index_pos*6),56-24,0);
};
void set_char()
{
		index_char++;
		index_char%=26;
		passwd[index_pos]='A'+index_char;
		GO_MENU_FUNC(UPDATE);// update_menu();
		OLED_PutChar_new(passwd[index_pos],121-(74+index_pos*6),56-24,0);
};

void set5(void)
{
	if  (strcmp(passwd,"RTFGHJA")==0) 
	{
	sprintf(str,"Password access");
 	OLED_PutText(20,48, FONT_1X,FONT_fine,1,str); //str to LCD
	flag_password_access=1;
	} else
	{
		sprintf(str,"Password error");
	 	OLED_PutText(20,48, FONT_1X,FONT_fine,1,str); //str to LCD
	};
};




void set6(void)
{
};

void update_log(void)
{
 Menu_Item* p;
 unsigned char i,j;
 short *data;
 short tmp_data;

 OLED_PutText_flash(0,0, FONT_1X,FONT_fine,1,MENU_TEXT); //flash str to LCD
 DrawLine(128,64-10,0,64-10,1);
 for (i=v_index_l,j=0;i<v_index_h;i++,j++)
  {
		sprintf(str,"%02d. %04d В ",i,v_array[i]);
		OLED_PutText(0,12+8*j, FONT_1X,FONT_fine,1,str); //str to LCD
  };
}

void set_inc()
{
 if (v_index_l>0)
 {
 	v_index_h--;
 	v_index_l--;
	GO_MENU_FUNC(UPDATE);
 };
};
void set_dec()
{
 if (v_index_h<250)
 {
 	v_index_h++;
 	v_index_l++;
	GO_MENU_FUNC(UPDATE);
 };
};





void set_cal_u_anode_inc(void)
{

	if (cal_u_anode<800) cal_u_anode+=2;;
	GO_MENU_FUNC(UPDATE);
};
void set_cal_u_anode_dec(void)
{
	if (cal_u_anode>300)  cal_u_anode-=2;;
	GO_MENU_FUNC(UPDATE);
};

void set_cal_k_i_inc(void)
{
	if (cal_I<250) cal_I++;
	GO_MENU_FUNC(UPDATE);
};
void set_cal_k_i_dec(void)
{
	if (cal_I>50) cal_I--;
	GO_MENU_FUNC(UPDATE);
};

void set_cal_k_u_inc(void)
{
	if (cal_U<150) cal_U++;
	GO_MENU_FUNC(UPDATE);
};

void set_cal_k_u_dec(void)
{
	if (cal_U>50) cal_U--;
	GO_MENU_FUNC(UPDATE);
};




void set_cal_m(void)
{	
	if (warm_flag==1)
	{	
		start_flag=1;
		dac1();
		sprintf(str,"установка             ");
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
	};	
	//Сохранить в eeprom или выставить в режим
};

void set_cal_s(void)
{
if (flag_ready_fire==1)
{
	if (test_off_on==0)
	{
	 write_dac((int)(4096.0*((double)cal_u_anode/1000.0)),2);
	 sprintf(str,"выстрел               ");
     OLED_PutText(0,48, FONT_1X,FONT_fine,1,str);	 //str to LCD  	
	 enable_handler(&main_timer0,7);
	 test_off_on=1;
	} else
	{
	  disable_handler(&main_timer0,7);
	  write_dac(0,2); 		// нет анодного тока
	  write_dac(dac_h1,1); // возвращение к прогретому состоянию
	  flag_ready_fire=0;
	  sprintf(str,"выстрел  окончен      ");
	  OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
	  test_off_on=0;	  	
	};
};
};

void set_cal_i(void)
{
 	store_param_cal();
	sprintf(str,"сохранение параметров");
	OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD
	//Сохранить в eeprom
};
void set_cal_u(void)
{ 
 	store_param_cal();
	sprintf(str,"сохранение параметров");
	OLED_PutText(0,48, FONT_1X,FONT_fine,1,str);//str to LCD
	//Сохранить в eeprom
};





void update_menu_cal_m1(void)
{
	update_menu();
	sprintf(str,"U=%4d.%1d%%",cal_u_anode/10,cal_u_anode%10); //str to LCD
	OLED_PutText(40,26, FONT_1X,FONT_fine,1,str);
	sprintf(str,"установить ?     ");
	OLED_PutText(0,48, FONT_1X,FONT_fine,1,str);	 //str to LCD	 	 	 
	//Дописать вывод текущего U в центр экрана
};
void update_menu_cal_m2(void)
{
	update_menu();
};

void update_menu_cal_i(void)
{
	update_menu();
	sprintf(str,"K=%1d.%02d",cal_I/100,cal_I%100);
	OLED_PutText(40,26, FONT_1X,FONT_fine,1,str); //str to LCD
	sprintf(str,"сохранение ?         ");
	OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD		 	 
	//Дописать вывод текущего U в центр экрана
};

void update_menu_cal_u(void)
{
	update_menu();
	sprintf(str,"K=%1d.%02d",cal_U/100,cal_U%100);
	OLED_PutText(40,26, FONT_1X,FONT_fine,1,str); //str to LCD
	sprintf(str,"сохранение ?         ");
	OLED_PutText(0,48, FONT_1X,FONT_fine,1,str); //str to LCD		 
 
	//Дописать вывод текущего U в центр экрана
};


void auto_fire(void)
{
};

void idle(void)
{
	void (*f_prt)(void);
	void *p_void;

	if (!empty(&main_message_list))
	{
		message_receive(&main_message_list,&p_void);    
	    f_prt=(void(*)(void))p_void;
	    (*f_prt)();
	};
};

void chech_uart(void)
{
  if (GetLenUART0()>0)
   {
   	reset_timer_handler(&main_timer0,5,-1);
	_delay_ms(2);
	GetStringUART0( str1, GetLenUART0());
	char command=atol(str1);
	cli();
	switch (command)
	{
	 case 1:
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,"Напряжение");
		//set_u_anode();
		//PurgeUART0();
	//	sprintf(str2,"Напряжение %04d\n",index_u_anode);
     break;

	 case 2:
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,"меню      ");
		menu();
		//PurgeUART0();
		//sprintf(str2,"cmd2!");
     break;

	 case 3:
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,"время     ");
		//set_t_exposure();
		//PurgeUART0();
		//sprintf(str2,"Время %d04\n",index_del_t);
     break;

	 case 4:
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,"установить");
		set();
		//PurgeUART0();
		//sprintf(str2,"cmd4!");
     break;

	 case 5:
		OLED_PutText(0,48, FONT_1X,FONT_fine,1,"выстрел   ");
		fire();
		//PurgeUART0();
		//sprintf(str2,"cmd5!");
     break;
	}; 
	sei();

	PutStringUART0(str2);	
   };

};

