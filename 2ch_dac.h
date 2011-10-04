#ifndef  __2CH_DAC_H__
#define  __2CH_DAC_H__

#define CS_2DAC_DDR 		DDRB
#define CS_2DAC_PORT 		PORTB
#define CS_2DAC_PIN			PB5

#define LDAC_DDR		DDRB
#define LDAC_PORT		PORTB
#define LDAC_PIN		PB4

#define CS_2DAC_CONF 	CS_2DAC_DDR|=1<<CS_2DAC_PIN
#define CS_2DAC_DISABLE	CS_2DAC_PORT|=1<<CS_2DAC_PIN
#define CS_2DAC_ENABLE	CS_2DAC_PORT&=~(1<<CS_2DAC_PIN)

#define LDAC_2DAC_CONF 	    LDAC_DDR|=1<<LDAC_PIN
#define LDAC_2DAC_SET    	LDAC_PORT|=1<<LDAC_PIN
#define LDAC_2DAC_CLR    	LDAC_PORT&=~(1<<LDAC_PIN)


#define DAC1 0
#define DAC2 1

#define REF_UNBUFF 	0
#define REF_BUFF 		1

#define MODE0		0
#define MODE1		1
#define MODE2		2
#define MODE3		3



void Init_2Dac(void);


void ReInit_2Dac(void);


void transmit_2dac(unsigned char channel,unsigned char buf,unsigned char mode,unsigned short data);


#endif
