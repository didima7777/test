#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "button.h"
#include "2ch_dac.h"

list_button  main_button={NULL,NULL,NULL};

void set_handler_anykey(p_list_button p,type_pointer_handler _handler_default)
{
	p->handler_default=_handler_default;
};

void add_button(p_list_button p,unsigned  char _number,short* _port,unsigned char _pin,type_pointer_handler _handler_down,type_pointer_handler _handler_press,type_pointer_handler _handler_up)
{
	p_conf_button tmp;
	
	if (p->head!=NULL)
	{
		cli();
			tmp=(p_conf_button)calloc(sizeof(conf_button),1);
				tmp->number=_number;
				tmp->port=_port;
				tmp->count=0;
				tmp->count_press=0;
				tmp->btn.event=0;
				tmp->btn.press=0;
				tmp->btn.pin=_pin;
				tmp->handler_up=_handler_up;
				tmp->handler_down=_handler_down;
				tmp->handler_press=_handler_press;
				tmp->next=NULL;
				p->tail->next=tmp;
				p->tail=tmp;
		sei();
	} else
	{
		tmp=(p_conf_button)calloc(sizeof(conf_button),1);
		cli();
			tmp->port=_port;
			tmp->count=0;
			tmp->count_press=0;
			tmp->btn.event=0;
			tmp->btn.press=0;
			tmp->btn.pin=_pin;
			tmp->handler_up=_handler_up;
			tmp->handler_down=_handler_down;
			tmp->handler_press=_handler_press;
			tmp->next=NULL;
			p->head=tmp;
			p->tail=tmp;
		sei();
	};
}


void check_buttons(p_list_button p)
{
	p_conf_button tmp;

	tmp=p->head;
	while(tmp!=NULL)
	{
		if (!(*((unsigned char*)((unsigned char*)tmp->port))&(1<<tmp->btn.pin)))
		{			
		  if (tmp->count>25)	
		  {
			if (tmp->btn.press==0)
			{
				tmp->btn.press=1;
				tmp->btn.event=1;
				if (tmp->handler_down!=NULL) tmp->handler_down();
				if (p->handler_default!=NULL) p->handler_default();
			}; 
			if (tmp->btn.press==1)
			{
				tmp->count_press++;
				if(tmp->count_press>5)  //45
				{
				 if (tmp->handler_press!=NULL) tmp->handler_press();				
				 tmp->count_press=0;
				};
			};
		  } else tmp->count++;
		} 
		else
		{
		  if (tmp->count==0)
		   {
			 if (tmp->btn.press==1) 
			 {
			  tmp->btn.press=0;
			  tmp->count_press=0;
			  if (tmp->handler_up!=NULL) tmp->handler_up();
			 };
		   } else tmp->count--;
		};
	  tmp=tmp->next;
	};
};


unsigned char get_event_button(p_list_button p,unsigned  char _number)
{
p_conf_button tmp;	

	tmp=p->head;

	while(tmp!=NULL)
	{
	 if (tmp->number==_number) 
	 { 
		if (tmp->btn.event==1)
		{
		 tmp->btn.event=0;
		 return 1;
		} else return 0;
		
	 }
	 tmp=tmp->next;
	};
	return 0;
};

unsigned char get_press_button(p_list_button p,unsigned  char _number)
{
p_conf_button tmp;	

	tmp=p->head;

	while(tmp!=NULL)
	{
	 if (tmp->number==_number) 
	 { 
		if (tmp->btn.press==1) return 1; else return 0;

	 }
	 tmp=tmp->next;
	};


	return 0;
};
