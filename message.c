#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "message.h"

list_message main_message_list={NULL,NULL,0};

void Init_message(list_message *list)
{
 list->first = list->last = NULL;
 list->size=0;
};

void message_send(list_message *list,void *msg)
{
 	message* node;
	

	node=(message*)calloc(sizeof(message),1);
	node->data=msg;
	node->next=NULL;
cli();
    if ( list->first == NULL )
	 {
	    list->first=node;
		list->last=node;
		list->size=1;
	 }
    else 
	 {
        list->last->next = node;
        list->last = node;
		list->size++;
     }
sei();
};

void message_receive(list_message *list,void **msg)
{
    
	message* node;

	if (list->first==NULL) 
	{
        *msg=NULL;
		return ;
    }
cli();
    node = list->first;
	*msg=node->data;
    if (list->last == list->first)
	{
    	list->first = list->last = NULL;
		list->size=0;
	}
    else
	{
        list->first = list->first->next;
		list->size--;
	};
    free(node);
sei();
};

unsigned char empty(list_message *list)
{
 if (list->size>0) return 0; else return 1;
}
