/*
 * serial.c
 *
 * Created: 10/8/2015 11:13:45 PM
 *  Author: Admin
 */
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "serial.h"#include "semphr.h"#include "BareBones/BareBonesWHT.h"MoteData_t moteData;
SemaphoreHandle_t frameHandle;




/* Constants for writing to UCSRB. */
#define serRX_INT_ENABLE				( ( unsigned char ) (1<<RXCIE0) )#define serRX_ENABLE					( ( unsigned char ) (1<<RXEN0) )#define serTX_ENABLE					( ( unsigned char ) (1<<TXEN0) )#define serTX_INT_ENABLE				( ( unsigned char ) (1<<TXCIE0) )
static QueueHandle_t xCharsForTx;


#define BAUDRATEMOTE 115200                           // define baud for WH mote
#define UBRRMOTE ((configCPU_CLOCK_HZ)/(BAUDRATEMOTE*16UL)-1)            // set baud rate value for UBRR

xComPortHandle xSerialPortInitMinimal( unsigned portBASE_TYPE uxQueueLength ){	
	
	portENTER_CRITICAL();
	/* Create the queues used by the com test task. */
		//xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
		xCharsForTx = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
		UBRR0 = UBRRMOTE;
		//UCSR0A = (1<<U2X0);
		UCSR0B = (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0);                // enable receiver and transmitter
		UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);   // 8bit data format
		//Enable interrupts for  TX later
		
	frameHandle = xSemaphoreCreateBinary();
	portEXIT_CRITICAL();
	
	/* Unlike other ports, this serial code does not allow for more than one
	com port.  We therefore don't return a pointer to a port structure and can
	instead just return NULL. */
	return NULL;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
	/* Only one port is supported. */
	( void ) pxPort;

	/* Return false if after the block time there is no room on the Tx queue. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{		
		return pdFAIL;
	}	

	//vInterruptOn();	UCSR0B |= (1<<UDRIE0);
	return pdPASS;
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/ISR( USART_RX_vect ) __attribute__ ((hot, flatten));
ISR( USART_RX_vect )
{
signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	/* Get the character and post it on the queue of Rxed characters.
	If the post causes a task to wake, force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */
	//PORTB ^= (1<<PORTB1);
	
	
	if( InterruptReception(&moteData)){//check to see if a valid frame has been received
		
		xSemaphoreGiveFromISR(frameHandle,NULL);
		
	}
	
	
	
	if( xHigherPriorityTaskWoken != pdFALSE )
	{
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/
ISR( USART_UDRE_vect ) __attribute__ ((hot, flatten));
ISR( USART_UDRE_vect )
{
signed char cChar, cTaskWoken;PORTB |= (1<<PORTB2);
	if( xQueueReceiveFromISR( xCharsForTx, &cChar,NULL) == pdTRUE )
	{
		/* Send the next character queued for Tx. */    	UDR0 = cChar;
	}
	else
	{/* Queue empty, nothing to send. */	//vInterruptOff();
	UCSR0B &= ~(1<<UDRIE0);		
	}
}void serialMoteSend(int8_t tx){		xSerialPutChar(NULL,tx,0x0010);	}int8_t serialMoteReceive(void){	return UDR0;}uint8_t sendMoteData(uint8_t* data, uint8_t data_size){		return sendData(data,data_size,serviceID,&moteData);		}void configureMote(void){			moteData.serialMoteSend=serialMoteSend; 	moteData.serialMoteReceive=serialMoteReceive;		DDRC = (1<<PORTC0)|(1<<PORTC1); // select pc0 and pc1 as outputs
	//pc0 drives the reset on the whmt, pc1 is the led indication
	
	PORTC = (1<<PORTC2);//Pull up on the button, reset the mote
	
	vTaskDelay(200);
	
	PORTC |= (1<<PORTC0);// turn on the mote
	
	MoteInitialization(&moteData);
	MoteSvcInitialization(&moteData);
	
	PORTC |= (1<<PORTC1); //turn on LED, the Mote is ready to send	}

