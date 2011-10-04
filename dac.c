#include<avr/io.h>
#include "2ch_dac.h"

void Init_dac(void)
 { 
 	Init_2Dac();
 };

void write_dac(unsigned short I,unsigned short num_mod)
 {    
   if (num_mod==1) transmit_2dac(DAC1,REF_BUFF,MODE0,I);
   if (num_mod==2) transmit_2dac(DAC2,REF_BUFF,MODE0,I);
 };
