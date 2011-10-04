#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include"handler.h"

list_handler main_timer0={NULL,NULL};

void Init_Timer0(void)
{
 OCR0=127;//125 for 8 MHz
 TCCR0|=1<<WGM01|1<<CS01|1<<CS00;
 TIMSK|=1<<OCIE0;
};


void handler_default(void)
{
 asm("nop");
};


void Init_handler(p_list_handler p)
{
	p_timer_handler tmp;
	
	tmp=(p_timer_handler)calloc(sizeof(timer_handler),1);
	cli();
		tmp->number=0;
		tmp->enable=1;
		tmp->top=10;
		tmp->count=0;
		tmp->repet=-1;
		tmp->handler=handler_default;
		tmp->next=NULL;
	p->head=tmp;
	p->tail=tmp;
	sei();
};


void add_handler(p_list_handler p,short _top,type_handler f,unsigned char _number,short _repet)
{
	p_timer_handler tmp;
	
	cli();
		tmp=(p_timer_handler)calloc(sizeof(timer_handler),1);
			tmp->number=_number;
			tmp->enable=1;
			tmp->top=_top;
			tmp->count=0;
			tmp->repet=_repet;
			tmp->handler=f;
			tmp->next=NULL;
		p->tail->next=tmp;
		p->tail=tmp;
	sei();
};


void reset_timer_handler(p_list_handler p,unsigned char _number,short _repet)
{
p_timer_handler tmp;	

	tmp=p->head;

	cli();
	while(tmp!=NULL)
	{
	 if (tmp->number==_number) 
	 { 
	  tmp->count=0;
	  tmp->repet=_repet;
	 }
	 tmp=tmp->next;
	};
	sei();
};

short get_count_handler(p_list_handler p,unsigned char _number)
{
p_timer_handler tmp;	

	tmp=p->head;
	cli();
	while(tmp!=NULL)
	{
	 if (tmp->number==_number) 
	 { 
	  return tmp->count;
	 }
	 tmp=tmp->next;
	};
	sei();
	return -1;
};


void disable_all_only(p_list_handler p,unsigned char _number)
{
p_timer_handler tmp;	

	tmp=p->head;

	while(tmp!=NULL)
	{
	 if (tmp->number!=_number) 
	 { 
	   tmp->enable=0;
	 }
	 tmp=tmp->next;
	};
};

void disable_handler(p_list_handler p,unsigned char _number)
{
p_timer_handler tmp;	

	tmp=p->head;

	while(tmp!=NULL)
	{
	 if (tmp->number==_number) 
	 { 
	   tmp->enable=0;
	 }
	 tmp=tmp->next;
	};

};

void enable_handler(p_list_handler p,unsigned char _number)
{
p_timer_handler tmp;	

	tmp=p->head;

	while(tmp!=NULL)
	{
	 if (tmp->number==_number) 
	 { 
	   tmp->enable=1;
	 }
	 tmp=tmp->next;
	};

};


void enable_all(p_list_handler p)
{
p_timer_handler tmp;	

	tmp=p->head;

	while(tmp!=NULL)
	{
     tmp->enable=1;
	 tmp=tmp->next;
	};
};


SIGNAL(TIMER0_COMP_vect)
{
p_timer_handler tmp;	

PORTD|=1<<PD7;

	tmp=main_timer0.head;

	while(tmp!=NULL)
	{
	 if (tmp->enable==1)
	 {
	  if (tmp->count==tmp->top) 
		 {
		  tmp->count=0;
		  if (tmp->repet<0) tmp->handler();
		  if (tmp->repet>0)
			{
				tmp->handler();	
				tmp->repet--;
			};
	  	 } else tmp->count++;
	 };
	 tmp=tmp->next;
	}

PORTD&=~(1<<PD7);

};
