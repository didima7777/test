/*
	           MICRO-MENU

	     (C) Dean Camera, 2007
	    www.fourwalledcubicle.com
	dean_camera@fourwalledcubicle.com
	
	    Royalty-free for all uses.
	                                  */

#ifndef MENU_H
#define MENU_H

#include <avr/pgmspace.h>


// Typedefs:
typedef void (*FuncPtr)(void);
typedef void (*WriteFuncPtr)(const char*);

typedef struct PROGMEM{
	void       *Next;
	void       *Previous;
	void       *Parent;
	void       *Sibling;
	FuncPtr     SelectFunc1;
	FuncPtr     SelectFunc2;
	FuncPtr     Update;
	FuncPtr     EnterFunc;	
	unsigned short 	   *data;
	const char  Text[];
} Menu_Item ;



// Externs:
extern WriteFuncPtr*    WriteFunc;
extern Menu_Item        Null_Menu;
extern Menu_Item*       CurrMenuItem;

// Defines and Macros:
#define NULL_ENTRY Null_Menu
#define NULL_FUNC  (void*)0
#define NULL_TEXT  0x00


#define PREVIOUS   	((Menu_Item*)pgm_read_word(&CurrMenuItem->Previous))
#define NEXT       	((Menu_Item*)pgm_read_word(&CurrMenuItem->Next))
#define PARENT     	((Menu_Item*)pgm_read_word(&CurrMenuItem->Parent))
#define SIBLING    	((Menu_Item*)pgm_read_word(&CurrMenuItem->Sibling))
#define SELECTFUNC1 ((FuncPtr*)pgm_read_word(&CurrMenuItem->SelectFunc1))
#define SELECTFUNC2 ((FuncPtr*)pgm_read_word(&CurrMenuItem->SelectFunc2))
#define UPDATE 		((FuncPtr*)pgm_read_word(&CurrMenuItem->Update))
#define ENTERFUNC  	((FuncPtr*)pgm_read_word(&CurrMenuItem->EnterFunc))
#define MENU_DATA 	((unsigned short*)pgm_read_word(&CurrMenuItem->data))
#define MENU_TEXT 	((unsigned char*)(CurrMenuItem->Text))


#define MAKE_MENU(Name, Next, Previous, Parent, Sibling, SelectFunc1, SelectFunc2, Update, EnterFunc,	data,Text) \
    extern Menu_Item Next;     \
	extern Menu_Item Previous; \
	extern Menu_Item Parent;   \
	extern Menu_Item Sibling;  \
	Menu_Item Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Sibling, (FuncPtr)SelectFunc1, (FuncPtr)SelectFunc2,(FuncPtr)Update, (FuncPtr)EnterFunc,(unsigned short*)&data,{Text}}

#define SET_MENU_WRITE_FUNC(x) \
	WriteFunc = (WriteFuncPtr*)x;

#define SET_MENU(x) \
	MenuChange((Menu_Item*)x);
	
#define GO_MENU_FUNC(x)  \
	MenuFunc((FuncPtr*)x)
	
#define EXTERN_MENU(Name) \
    extern Menu_Item Name;

// Prototypes:
void MenuChange(Menu_Item* NewMenu);
void MenuFunc(FuncPtr* Function);
void MenuInit (void);


#endif
