/*
 * commTask.c
 *
 * Created: 10/9/2015 12:00:01 AM
 *  Author: Admin
 */ 

#include "FreeRTOS.h"#include "task.h"
#include "commTask.h"
#include "queue.h"
#include "semphr.h"
#include "Drivers/serial.h"
#include "SmartSit.h"  // Definitions for SmartSit project

#define BUFFER_LENGTH 128
#define comSTACK_SIZE		256
/* The Rx task will block on the Rx queue for a long period. */
#define comRX_BLOCK_TIME			( ( TickType_t ) 0xffff )
/* We should find that each character can be queued for Tx immediately and we
don't have to block to send. */
#define comNO_BLOCK					( ( TickType_t ) 0x0010 )
/* Handle to the com port used by both tasks. */

#define DATA_READY_TO_SEND_BLOCK_TIME		( ( TickType_t ) 0xffff )	
/* We should block a long time for the data to be ready */


static xComPortHandle xPort = NULL;

extern QueueHandle_t DataToSend;
SemaphoreHandle_t moteReadySemaphore; // sync for sensor task

void startCommTasks(UBaseType_t uxPriority){
	xSerialPortInitMinimal(BUFFER_LENGTH);
	/* The Tx task is spawned with a lower priority than the Rx task. */
	moteReadySemaphore = xSemaphoreCreateBinary();
	xTaskCreate( xMoteTask,( signed char * ) "Mote", comSTACK_SIZE, NULL, uxPriority, NULL );
		
}

void xMoteTask( void* pvParameters){
	
	
	configureMote();
	xSemaphoreGive(moteReadySemaphore);
	
	uint8_t *datos;  //Pointer to data to be sent to the mote and to be received from the sensor queue
	
	
	
	//PORTB |= (1<<PORTB5);
	//uint8_t datos2[]={'D','I','N','E','R','O'};
	
	
	//PORTB |= (1<<PORTB5);
	for ( ;; )
	{
		//sendMoteData(datos2,sizeof(datos2));
		//vTaskDelayUntil(&xLastWakeTime2,xFrequency2);
		
		if ( xQueueReceive(DataToSend,&(datos),DATA_READY_TO_SEND_BLOCK_TIME)  ) //Block on the DataToSendQueue
		{
			//The queue is of size 1 and contains the pointer to the data to be sent
			sendMoteData(datos,NUMBER_OF_BYTES_TO_SEND);
		}
		
		
	}
	
}






