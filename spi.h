#ifndef _SPI_H_
#define _SPI_H_


#define SPI_DDR	 DDRB
#define SPI_MOSI PB2
#define SPI_SCK	 PB1
#define SPI_SS	 PB0

#define SPI_PORT_INIT	SPI_DDR|=1<<SPI_SCK|1<<SPI_MOSI|1<<SPI_SS;



void Init_SPI(unsigned char conf,unsigned char speed);
void ReInit_SPI(unsigned char conf,unsigned char speed);
unsigned char transmit_spi(unsigned char byte);

#endif
