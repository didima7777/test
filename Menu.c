/*
	           MICRO-MENU

	     (C) Dean Camera, 2007
	    www.fourwalledcubicle.com
	dean_camera@fourwalledcubicle.com
	
	    Royalty-free for all uses.
	                                  */

#include "Menu.h"
#include <avr/interrupt.h>

#include<math.h>
#include  "work_func.h"

//                 NEXT,      PREVIOUS     		PARENT,     CHILD		 SELECTFUNC1	      SELECTFUNC2							ENTERFUNC		VALUE
MAKE_MENU(m_s1i1,  m_s1i2,    m_s1i6,  	   	   NULL_ENTRY, m_s2i1,       NULL_FUNC, 	      set1,		    update_menu_for_main,	menu_next_1,	NULL_ENTRY,"Основное меню \0");
							  //заменить на m_s1i5
							  
MAKE_MENU(m_s1i2,  m_s1i3,    NULL_ENTRY,      NULL_ENTRY, m_s3i1,       NULL_FUNC, 	      set2,		    update_menu,			menu_next_1,	NULL_ENTRY,"Настройки накала  \0");
MAKE_MENU(m_s1i3,  m_s1i4,    NULL_ENTRY,      NULL_ENTRY, m_s4i1,       NULL_FUNC, 	      NULL_FUNC,    update_menu,			menu_next_1,	NULL_ENTRY,"Настройка режимов \0");
MAKE_MENU(m_s1i4,  m_s1i6,    NULL_ENTRY,      NULL_ENTRY, m_s7i1,       NULL_FUNC, 	      set4,		    update_menu_test,		menu_next_1,	NULL_ENTRY,"Тестирование \0");

MAKE_MENU(m_s1i6,  m_s1i7,	  m_s1i7,      	   NULL_ENTRY, m_s9i1,   	 NULL_FUNC, 	      set6,		    update_log,				menu_next_1,	NULL_ENTRY,"Измерения \0");
MAKE_MENU(m_s1i7,  m_s1i1,    m_s1i5,      	   NULL_ENTRY, m_s10i1,      NULL_FUNC, 	      NULL_FUNC,    update_menu,			menu_next_1,	NULL_ENTRY,"Калибровка \0");

MAKE_MENU(m_s1i5,  m_s1i1,	  m_s1i1,      	   NULL_ENTRY, m_s8i1,       NULL_FUNC, 	      set5,		    update_passwd,			menu_next_1,	passwd,    "Ввод пароля \0");


// подменю выстрела
MAKE_MENU(m_s2i1,  m_s2i2,	  NULL_ENTRY,      m_s1i1,     NULL_ENTRY,   set_mode,   	      NULL_FUNC,	NULL_FUNC,				NULL_FUNC,	index_mode,  "Режим \0");
MAKE_MENU(m_s2i2,  NULL_ENTRY,m_s2i1,  	   	   m_s1i1,     NULL_ENTRY,   set_group,		      NULL_FUNC,	NULL_FUNC,		    	NULL_FUNC,	index_group,"Группа \0");
 
// подменю настройки накала
MAKE_MENU(m_s3i1,  m_s3i2,    NULL_ENTRY, 	   m_s1i2,     NULL_ENTRY,   set_dac_h1, 	      NULL_FUNC,	NULL_FUNC,				NULL_FUNC,	dac_h1,"Дежурный \0");
MAKE_MENU(m_s3i2,  NULL_ENTRY,m_s3i1,  	   	   m_s1i2,     NULL_ENTRY,   set_dac_h2,	      NULL_FUNC,	NULL_FUNC,				NULL_FUNC,	dac_h2,"Рабочий \0");

// подменю настройки накала
MAKE_MENU(m_s4i1,  m_s4i2	 ,m_s4i2,  	   	   m_s1i3,     m_s5i1,       goto_menu1,	      NULL_FUNC,	update_menu_for_main,	menu_next_2,NULL_ENTRY,"Выбор номера режимов \0");
MAKE_MENU(m_s4i2,  NULL_ENTRY,m_s4i1,  	       m_s1i3,     m_s6i1,       goto_menu2,	      set3,		    update_menu_edit,		menu_next_2,NULL_ENTRY,"Редактирование режима \0");
		
// просмотр режимов
MAKE_MENU(m_s5i1,  m_s5i2,    NULL_ENTRY,      m_s4i1,     NULL_ENTRY,   set_num_rate,	      NULL_FUNC,	NULL_FUNC,				NULL_FUNC,	num_rate,	"Группа \0");
MAKE_MENU(m_s5i2,  NULL_ENTRY,m_s5i1,  	       m_s4i1,     NULL_ENTRY,   set_num_group,	      NULL_FUNC,	NULL_FUNC,				NULL_FUNC,	num_group,  "Режим \0");

// Добавление режима
MAKE_MENU(m_s6i1,  m_s6i2,    NULL_ENTRY,      m_s4i2,     NULL_ENTRY,   set_new_u_anode,	  NULL_FUNC,	NULL_FUNC, 				NULL_FUNC,	new_u_anode,"Напряжение \0");
MAKE_MENU(m_s6i2,  NULL_ENTRY,m_s6i1,  	       m_s4i2,     NULL_ENTRY,   set_new_t_exposure,  NULL_FUNC,    NULL_FUNC,				NULL_FUNC,	new_del_t,"Время \0");

// Тестирование
MAKE_MENU(m_s7i1,  m_s7i2,    NULL_ENTRY,      m_s1i4,     NULL_ENTRY,   set_test_u_anode,	  NULL_FUNC,	NULL_FUNC, 			    NULL_FUNC,	test_u_anode,"Напряжение \0");
MAKE_MENU(m_s7i2,  NULL_ENTRY,m_s7i1,  	       m_s1i4,     NULL_ENTRY,   set_test_t_exposure, NULL_FUNC,    NULL_FUNC,			    NULL_FUNC,	NULL_ENTRY,	 "Выход на раб. режим\0");

//Ввод пароля
MAKE_MENU(m_s8i1,  m_s8i2,    NULL_ENTRY,      m_s1i5,     NULL_ENTRY,   set_pos,	 		  NULL_FUNC,	NULL_FUNC, 		        NULL_FUNC,	test_u_anode,"Напряжение \0");
MAKE_MENU(m_s8i2,  NULL_ENTRY,m_s8i1,  	       m_s1i5,     NULL_ENTRY,   set_char,			  NULL_FUNC,    NULL_FUNC,	   	        NULL_FUNC,	NULL_ENTRY,	 "Выход на раб. режим\0");

//Лог измерения
MAKE_MENU(m_s9i1,  m_s9i2,    NULL_ENTRY,      m_s1i6,     NULL_ENTRY,   set_inc,	 	 	  NULL_FUNC,	NULL_FUNC, 			    NULL_FUNC,	NULL_ENTRY,	 "Вверх \0");
MAKE_MENU(m_s9i2,  NULL_ENTRY,m_s9i1,  	       m_s1i6,     NULL_ENTRY,   set_dec,			  NULL_FUNC,    NULL_FUNC,				NULL_FUNC,	NULL_ENTRY,	 "Вниз  \0");

// Калибровка
MAKE_MENU(m_s10i1,  m_s10i2,    NULL_ENTRY,    m_s1i7,      m_s11i1,   	  goto_menu1,	  	   set_cal_m,	update_menu_cal_m1, 	menu_next_2,NULL_ENTRY,"Режим калибровки \0");
MAKE_MENU(m_s10i2,  NULL_ENTRY,m_s10i1,  	   m_s1i7,      m_s12i1,      goto_menu2,		   set_cal_s,	update_menu_cal_m2,		menu_next_2,NULL_ENTRY,"Калибровка \0");

//Режим калибровки задание U анода
MAKE_MENU(m_s11i1,  m_s11i2,    NULL_ENTRY,    m_s10i1,     NULL_ENTRY,   set_cal_u_anode_inc, NULL_FUNC,	NULL_FUNC, 				NULL_FUNC,	NULL_ENTRY,	 "Вверх \0");
MAKE_MENU(m_s11i2,  NULL_ENTRY,m_s11i1,  	   m_s10i1,     NULL_ENTRY,   set_cal_u_anode_dec, NULL_FUNC,   NULL_FUNC,				NULL_FUNC,	NULL_ENTRY,	 "Вниз  \0");

//Пордменю выбора калибруймого параметра
MAKE_MENU(m_s12i1,  m_s12i2,    NULL_ENTRY,    m_s10i2,     m_s13i1,      goto_menu1,	 	   set_cal_i,	update_menu_cal_i, 		menu_next_2,NULL_ENTRY,	 "Ток \0");
MAKE_MENU(m_s12i2,  NULL_ENTRY,m_s12i1,  	   m_s10i2,     m_s14i1,   	  goto_menu2,		   set_cal_u,	update_menu_cal_u,		menu_next_2,NULL_ENTRY,	 "Напряжение\0");

//Калибровка тока
MAKE_MENU(m_s13i1,  m_s13i2,    NULL_ENTRY,    m_s12i1,     NULL_ENTRY,   set_cal_k_i_inc,     NULL_FUNC,	NULL_FUNC, 				NULL_FUNC,	NULL_ENTRY,	 "Вверх \0");
MAKE_MENU(m_s13i2,  NULL_ENTRY,m_s13i1,  	   m_s12i1,     NULL_ENTRY,   set_cal_k_i_dec,     NULL_FUNC,   NULL_FUNC,				NULL_FUNC,	NULL_ENTRY,	 "Вниз  \0");

//Калибровка напряжения
MAKE_MENU(m_s14i1,  m_s14i2,    NULL_ENTRY,    m_s12i2,     NULL_ENTRY,   set_cal_k_u_inc,     NULL_FUNC,	NULL_FUNC, 				NULL_FUNC,	NULL_ENTRY,	 "Вверх \0");
MAKE_MENU(m_s14i2,  NULL_ENTRY,m_s14i1,  	   m_s12i2,     NULL_ENTRY,   set_cal_k_u_dec,     NULL_FUNC,   NULL_FUNC,				NULL_FUNC,	NULL_ENTRY,	 "Вниз  \0");

//Добавить меню

Menu_Item        Null_Menu = {(void*)0, (void*)0, (void*)0, (void*)0, (void*)0,(void*)0,(void*)0,(void*)0,(unsigned short*)0x00, {0x00}};

Menu_Item*       CurrMenuItem;
WriteFuncPtr*    WriteFunc;


void MenuChange(Menu_Item* NewMenu)
{
	if ((void*)NewMenu == (void*)&NULL_ENTRY)
	  return;

	CurrMenuItem = NewMenu;
	
}

void MenuFunc(FuncPtr* Function)
{
	if ((void*)Function == (void*)NULL_FUNC)
	  return;

	((FuncPtr)Function)();
}

void MenuInit (void)
{
  SET_MENU(&m_s1i1);
}


