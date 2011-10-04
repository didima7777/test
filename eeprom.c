#include <avr/io.h>
#include <avr/interrupt.h>
#include "work_func.h"

//extern volatile unsigned short 	dac_h1;
//extern volatile unsigned short 	dac_h2;


void EEPROM_write (unsigned int uiAddress, unsigned char ucData)
{
 while (EECR & (1 << EEWE));
 EEAR = uiAddress;
 EEDR = ucData;
 EECR |= (1 << EEMWE);
 EECR |= (1 << EEWE);
}

unsigned char EEPROM_read (unsigned int uiAddress)
{
 while (EECR & (1 << EEWE));
 EEAR = uiAddress;
 EECR |= (1 << EERE);
 return EEDR;
}

void store_param(void)
{

 EEPROM_write (0,dac_h1/256);
 EEPROM_write (1,dac_h1%256);

 EEPROM_write (2,dac_h2/256);
 EEPROM_write (3,dac_h2%256);

};

void restore_param(void)
{
 unsigned char l_byte;
 unsigned char h_byte;

 h_byte=EEPROM_read (0);
 l_byte=EEPROM_read (1);
 dac_h1=256*h_byte+l_byte;
 if (dac_h1==0xffff) dac_h1=1500;

 h_byte=EEPROM_read (2);
 l_byte=EEPROM_read (3);
 dac_h2=256*h_byte+l_byte; 
	if (dac_h2==0xffff) dac_h2=2500;

};


void store_param_cal(void)
{

 EEPROM_write (4,cal_U/256);
 EEPROM_write (5,cal_U%256);

 EEPROM_write (6,cal_I/256);
 EEPROM_write (7,cal_I%256);

};

void restore_param_cal(void)
{
 unsigned char l_byte;
 unsigned char h_byte;

 h_byte=EEPROM_read (4);
 l_byte=EEPROM_read (5);
 cal_U=256*h_byte+l_byte;
 if (cal_U==0xffff) cal_U=71;

 h_byte=EEPROM_read (6);
 l_byte=EEPROM_read (7);
 cal_I=256*h_byte+l_byte; 
	if (cal_I==0xffff) cal_I=200;

};


void write_mode_U(unsigned char group,unsigned char mode,short value)
{
 short addr;

 addr=2*(group*4+mode)+10;
 EEPROM_write(addr,value/256);
 EEPROM_write(addr+1,value%256);

};

void write_mode_T(unsigned char group,unsigned char mode,short value)
{
 short addr;

 addr=2*(group*4+mode)+40;
 EEPROM_write(addr,value/256);
 EEPROM_write(addr+1,value%256);

};

short read_mode_U(unsigned char group,unsigned char mode)
{
 unsigned char l_byte;
 unsigned char h_byte;
 short addr;

 addr=2*(group*4+mode)+10;
 h_byte=EEPROM_read(addr);
 l_byte=EEPROM_read(addr+1);

 return 256*h_byte+l_byte;
};

short read_mode_T(unsigned char group,unsigned char mode)
{
 unsigned char l_byte;
 unsigned char h_byte;
 short addr;

 addr=2*(group*4+mode)+40;
 h_byte=EEPROM_read(addr);
 l_byte=EEPROM_read(addr+1);

 return 256*h_byte+l_byte;
};




