#ifndef  __QUEUE_FIFO_H__
#define  __QUEUE_FIFO_H__

#include"include.h"

typedef struct _queue_fifo_node_handler* p_queue_fifo_node_handler;
typedef struct _queue_fifo_node_handler  queue_fifo_node_handler;

struct _queue_fifo_node_handler
{
 unsigned char  number;
 type_pointer_handler handler;
 p_queue_fifo_node_handler next;
};

typedef struct _list_queue_fifo_handler* p_list_queue_fifo_handler;
typedef struct _list_queue_fifo_handler  list_queue_fifo_handler;

struct _list_queue_fifo_handler
{
	p_queue_fifo_node_handler head;
	p_queue_fifo_node_handler tail;
};


void Init_queue(p_list_queue_fifo_handler p);
void put_queue(p_list_queue_fifo_handler p,type_handler f,unsigned char _number);
void get_queue(p_list_queue_fifo_handler p);

#endif
