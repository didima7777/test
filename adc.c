#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"


unsigned char flag_new_data=0;

void Init_adc(void)
 {
 	ADCSRA|=1<<ADEN|1<<ADPS2|1<<ADPS1|0<<ADPS0;  

 };



unsigned short read_adc(unsigned char adc_mux)
 {
  
    ADMUX=adc_mux;
    ADCSRA |= 1<<ADSC;
 
    while (!(ADCSRA & (1<<ADIF)));	
    ADCSRA |= 1<<ADIF;

    return ADCW&0x3FF;;
 };



