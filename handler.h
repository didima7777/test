#ifndef  __HANDLER_H__
#define  __HANDLER_H__

#include"include.h"

typedef struct _timer_handler* p_timer_handler;
typedef struct _timer_handler  timer_handler;

struct _timer_handler
{
 unsigned char  number;
 unsigned char  enable;
 short count;
 short top;
 short repet;
 type_pointer_handler handler;
 p_timer_handler next;
};

typedef struct _list_handler* p_list_handler;
typedef struct _list_handler  list_handler;

struct _list_handler
{
	p_timer_handler head;
	p_timer_handler tail;
};

extern list_handler main_timer0;

void Init_Timer0(void);
void Init_handler(p_list_handler p);
void add_handler(p_list_handler p,short _top,type_handler f,unsigned char _number,short _repet);
void handler_default(void);
void reset_timer_handler(p_list_handler p,unsigned char _number,short _repet);
short get_count_handler(p_list_handler p,unsigned char _number);
void disable_all_only(p_list_handler p,unsigned char _number);
void disable_handler(p_list_handler p,unsigned char _number);
void enable_handler(p_list_handler p,unsigned char _number);
void enable_all(p_list_handler p);

#endif
