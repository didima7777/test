// UART.c

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "uart.h"



typedef struct
{
	/*--- General ---*/
	unsigned char EOS;
	
	/*--- Receive buffer ---*/
	 char *pRxBuffer;
	 char RxFirst;
	 char RxLast;
	 char RxCount;
	
	/*--- Transmit buffer ---*/
	 char *pTxBuffer;
	 char TxFirst;
	 char TxLast;
	 char TxCount;

} UARTDataType;

static  char TxBufferCh0[ TXSIZE_UART0 ];
static  char RxBufferCh0[ RXSIZE_UART0 ];
static volatile UARTDataType UART0;
extern unsigned char sign;
volatile unsigned char flag_manual = 1;

void UART0Init (void)
{
  	
	
	DDRD|=1<<PD3;
//инициализация UART
	UCSR1B |= (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1);
	//UCSRA |= (1<<U2X);
	UBRR1L = 12; //25 - 38400, 100-9600


	
	//Configure UART0 data block
	UART0.EOS = '\n'; // \n
	UART0.pTxBuffer   = TxBufferCh0;
	UART0.TxFirst     = 0x00;
	UART0.TxLast      = 0x00;
	UART0.TxCount     = 0x00;
	UART0.pRxBuffer   = RxBufferCh0;
	UART0.RxFirst     = 0x00;
	UART0.RxLast      = 0x00;
	UART0.RxCount     = 0x00;
	
}

void ResetUART0 ( void )
{
	;
}

void PutCharUART0( unsigned char Data )
{
	unsigned char Index;

	// Wait for an empty space in the Tx FIFO
	
	while ( UART0.TxCount == TXSIZE_UART0 )
    	{
        	;
    	}

	if ( ( UART0.TxCount == 0 ) && ( UCSR1A & (1<<UDRE) ) )
		{
			// Send character directly
			UDR1 = Data;
		}
	else
	{
		// Enter critical section
	  	cli();
	
		// Store in buffer
		Index = UART0.TxLast;
		UART0.pTxBuffer[ Index ] = Data;
	
		// Modify buffer pointers
		UART0.TxLast = (unsigned char)((UART0.TxLast + 1) % TXSIZE_UART0);
		UART0.TxCount++;
	
		// Enable transmit interrupt
		UCSR1B |= (1<<TXCIE);
	
		// Exit critical section
		sei();
	}
}

void PutStringUART0( char *pString )
{
	while( *pString )
	{
		PutCharUART0( (unsigned char)(*pString++) );
	}	
	return;
}

void PutDataUART0( unsigned char *pData, unsigned char Length )
{
	while( Length-- )
	{
		PutCharUART0( *pData++ ); 
	}
	return;
}

signed char GetCharUART0( unsigned char *pChar )
{

	unsigned char Index;

    	// Any characters in the Rx buffer?
		
	if( UART0.RxCount > 0 )
    	{
    		// Enter critical section
        	cli();

        	// Retrieve from buffer
        	Index = UART0.RxFirst;
        	*pChar = UART0.pRxBuffer[ Index ];

        	// Modify buffer pointers
        	UART0.RxFirst = (unsigned char)((UART0.RxFirst + 1) % RXSIZE_UART0);
        	UART0.RxCount --;

        	// Exit critical section
			sei();
    }
    else
    {
        	// The buffer is empty
                return STATUS_EMPTY;
    }

    return STATUS_OK;
}

signed char GetStringUART0( char *pString , unsigned char Size )
{
	unsigned char Count;
	signed char   Status;

	// Receive data bytes
	Count = 0;

	// Wait for received string
	do
	{
        	// Wait for received character
		do
		{
			Status = GetCharUART0( (unsigned char *)pString );
            
					if( Status == STATUS_ERROR )
            		{
                		// Append zero termination
			        	*pString = '\0';

		                // Problem detected
                		return STATUS_ERROR;
						
			}
		}
		while( Status != STATUS_OK );
	}
	while( ( *pString++ != UART0.EOS ) && ( ++Count < ( Size-1 ) ) );
	
	// Append zero termination
    	*pString = '\0';
	
	// Determine return code
	if( ++Count == Size )
    	{
        	return STATUS_FULL;
    	}
	
	return STATUS_OK;
}

signed char GetDataUART0( unsigned char *pData, unsigned char Length )
{
	signed char Status;

	// Wait for received data
	do
	{
		// Wait for received character
		do
		{
	    		Status = GetCharUART0( pData );
	    		if( Status == STATUS_ERROR )
	    		{	
				// Problem detected
				return STATUS_ERROR;
	    		}
		}
		while( Status != STATUS_OK );
	
		*pData++;
	}
	while( --Length );
	
	return STATUS_OK;
}

void SetEOStrUART0( unsigned char Symbol )
{
	// Set end of string symbol
    	UART0.EOS = Symbol;
}

SIGNAL(USART1_TX_vect) //передача
{
unsigned char Status;
unsigned char Index;

	//Read the line status code. It is necessary to clear interrupt.
			Status =  UCSR1A & (1<<UDRE) ;

			while ( Status && UART0.TxCount )
			{
				//Send a character
				Index = UART0.TxFirst;
				UDR1 = UART0.pTxBuffer[ Index ];

				//Modify buffer pointers
				UART0.TxFirst = (unsigned char)((UART0.TxFirst + 1) % TXSIZE_UART0);
           		UART0.TxCount--;	

				Status =  UCSR1A & (1<<UDRE) ;
			}

			//More data to send?
			if( !UART0.TxCount )
			{
				//Disable UART data register empty interrupt
				UCSR1B &= ~(1<<TXCIE);
			}
}

SIGNAL(USART1_RX_vect) //приём
{
//Receive data			
unsigned char temp;
unsigned char Index;

				//Read the character
				Index = UART0.RxLast;
                temp = UDR1;
            	UART0.pRxBuffer[ Index ] = temp;
                                

				//Modify buffer pointers
				UART0.RxLast = (unsigned char)((UART0.RxLast + 1) % RXSIZE_UART0);
            	UART0.RxCount++;
				
				if( UART0.RxCount > RXSIZE_UART0 )
				{
					//RX buffer full					
				}

                //if (temp == UART0.EOS)  UART_respond();

}

signed char GetLenUART0( void )
{
	return UART0.RxCount;
}

void PurgeUART0( void )
{
	UART0.TxFirst = 0x00;
	UART0.TxLast  = 0x00;
	UART0.TxCount = 0x00;
	
	UART0.RxFirst = 0x00;
	UART0.RxLast  = 0x00;
	UART0.RxCount = 0x00;
}

/*
unsigned char GetUART0Message(UARTMessage * RxMessage)
{
	unsigned char UART0Str[MESSAGE_LENGTH];

	if( GetLenUART0() < MESSAGE_LENGTH )
		return 0;
	else
	{
		if( GetDataUART0( UART0Str , MESSAGE_LENGTH ) == STATUS_OK )
		{
			RxMessage->DevID = UART0Str[0];
			RxMessage->CMD   = UART0Str[1];
			RxMessage->MSB16 = (UART0Str[2] | (UART0Str[3] << 8));
			RxMessage->LSB16 = (UART0Str[4] | (UART0Str[5] << 8));
			RxMessage->CRC   = (UART0Str[6] | (UART0Str[7] << 8));

			PurgeUART0();

			if( RxMessage->CRC == 0xB0BA )
			{
				if( RxMessage->DevID == ANAL_WORM_ID )
					return 1;
				else
					return 0;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			PurgeUART0();
			return 0;
		}
	}
}
*/
