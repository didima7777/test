#ifndef  __MACROS_H__
#define  __MACROS_H__

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL  // 1 MHz
#include <util/delay.h>

#define SetBit(x,y) (x|=(1<<y))
#define ClrBit(x,y) (x&=~(1<<y))


#define delay_us(us)  _delay_us(us)	
#define delay_ms(ms) _delay_ms(ms)	

#define __no_operation() asm("nop")
#define __flash PROGMEM

#endif
