#ifndef _MESSAGE_H_
#define _MESSAGE_H_


typedef struct _message message;


struct _message
{
 unsigned char id;
 void*		   data;
 message*	   next;
};


struct _list_message
{
 message 	   *first;
 message 	   *last;
 unsigned char size;
};

typedef struct _list_message list_message;

extern list_message main_message_list;

void Init_message(list_message *list);
void message_send(list_message *list,void *msg);
void message_receive(list_message *list,void **msg);
unsigned char empty(list_message *list);

#endif

