#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include"queue_fifo.h"
#include "include.h"

void Init_queue(p_list_queue_fifo_handler p)
{
	cli();
		p->head=NULL;
		p->tail=NULL;
	sei();
};

void put_queue(p_list_queue_fifo_handler p,type_handler f,unsigned char _number)
{
	p_queue_fifo_node_handler node;
	

		node=(p_queue_fifo_node_handler)calloc(sizeof(queue_fifo_node_handler),1);

		node->number = _number;
		node->handler=f;
		node->next = NULL;
cli();
    if (p->head == NULL)
	 {
	    p->head = p->tail = node;
	 }
    else 
	 {
        p->tail->next = node;
        p->tail = node;
     }
sei();
};

 
void get_queue(p_list_queue_fifo_handler p)
{
    type_pointer_handler node_handler;
	p_queue_fifo_node_handler tmp;

	if (p->tail==NULL) 
	{
        return ;
    }
cli();
    node_handler = p->tail->handler;
    tmp = p->head;
    if (p->head == p->tail)
        p->head = p->tail = NULL;
    else
        p->tail = p->tail->next;
    free(tmp);
sei();
node_handler();
    return ;
}
