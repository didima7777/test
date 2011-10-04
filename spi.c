#include <avr/io.h>
#include "spi.h"

void Init_SPI(unsigned char conf,unsigned char speed)
{ 
	SPI_PORT_INIT;
	PORTB|=1<<PB0;
	SPCR|=conf;

	if (speed) SPSR|=1<<SPI2X;
};

void ReInit_SPI(unsigned char conf,unsigned char speed)
{
	SPI_PORT_INIT;
	SPCR|=conf;
	if (speed) SPSR|=1<<SPI2X;
};

unsigned char transmit_spi(unsigned char byte)
{

 SPDR=byte;
 while(!(SPSR&(1<<SPIF)));

 return SPDR;
};
