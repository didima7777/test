#ifndef  __BUTTON_H__
#define  __BUTTON_H__

#include"include.h"

typedef struct _list_button* p_list_button;
typedef struct _list_button  list_button;


typedef struct _conf_button* p_conf_button;
typedef struct _conf_button  conf_button;

typedef struct _button  button;

struct _button
{
unsigned char pin:6;
unsigned char press:1;
unsigned char event:1;
};

struct _conf_button
{
 unsigned  char number;

 short 		count;
 unsigned char 		count_press;
 short*		port;
 button 	btn;

 type_pointer_handler handler_down;
 type_pointer_handler handler_up;
 type_pointer_handler handler_press;

 p_conf_button next;
};

struct _list_button
{
 p_conf_button head;
 p_conf_button tail;
 type_pointer_handler handler_default;
};

extern list_button  main_button;

void set_handler_anykey(p_list_button p,type_pointer_handler _handler_default);
void add_button(p_list_button p,unsigned  char _number,short* _port,unsigned char _pin,type_pointer_handler _handler_down,type_pointer_handler _handler_press,type_pointer_handler _handler_up);
void check_buttons(p_list_button p);

#endif
