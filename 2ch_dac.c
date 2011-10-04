#include<avr/io.h>
#include"spi.h"
#include "2ch_dac.h"


void Init_2Dac(void)
{ 
CS_2DAC_CONF;
LDAC_2DAC_CONF;
CS_2DAC_DISABLE;
Init_SPI(1<<SPE|1<<MSTR|1<<CPHA|0<<CPOL,1);
};


void ReInit_2Dac(void)
{ 
CS_2DAC_CONF;
LDAC_2DAC_CONF;
CS_2DAC_DISABLE;
Init_SPI(1<<SPE|1<<MSTR|1<<CPHA|0<<CPOL,1);
};

void transmit_2dac(unsigned char channel,unsigned char buf,unsigned char mode,unsigned short data)
{
 unsigned short tmp=0;

 tmp|=channel<<15|buf<<14|mode<<12|data<<0;

	CS_2DAC_ENABLE;
 transmit_spi((unsigned char)((tmp>>8)&0xff));
 transmit_spi((unsigned char)((tmp)&0xff));
	CS_2DAC_DISABLE;

LDAC_2DAC_SET;
LDAC_2DAC_CLR;
LDAC_2DAC_SET;
};

