/*
 * sensorTask.c
 *
 * Created: 10/9/2015 10:59:52 PM
 *  Author: Admin
 */ 
#include <avr/interrupt.h>
#include "FreeRTOS.h"#include "task.h"
#include "sensorTask.h"
#include "semphr.h"
#include "SmartSit.h"
#include "Drivers/TWI_Master.h"

#define SLAVE_SENSOR_W	0x3C
#define SLAVE_SENSOR_R 0x3D
#define CRA	0x00  //Configuration Regitser A
#define CRB 0x01	// Configuration Register B
#define MODE_REGISTER 0x02
#define DATA_OUTPUT_REGISTER 0x03
#define sensorSTACK_SIZE 256

extern SemaphoreHandle_t moteReadySemaphore; // sync for sensor task

SemaphoreHandle_t dataReadySemaphore; // data ready semaphore


QueueHandle_t DataToSend;


TaskHandle_t xSensorHandle;
void startSensorTasks(UBaseType_t uxPriority){
	
	xTaskCreate( xSensorTask,( signed char * ) "Sensor", sensorSTACK_SIZE, NULL, uxPriority, &xSensorHandle );
	
}

#ifdef DATA_READY_INTERRUPT


ISR( INT0_vect ) __attribute__ ((hot, flatten));
ISR( INT0_vect )
{
	xSemaphoreGiveFromISR(dataReadySemaphore,NULL);
}



void xSensorTask (void* pvParameters){
	
	static uint8_t sensorData[NUMBER_OF_BYTES_TO_SEND];	uint8_t messages=0;	uint8_t *pSensorData;			DataToSend = xQueueCreate(1,sizeof(uint8_t*));	//only 1 element in queue, pointer to data
	dataReadySemaphore = xSemaphoreCreateBinary();
	//create semaphore for dataReady, the sensorTask will block on this semaphore
	while(xSemaphoreTake(moteReadySemaphore,0xFFFF) != pdTRUE);
	//wait a long time for mote to be ready to send
	
	
	DDRD &= ~(1<<PORTD2); // as input
	PORTD |= (1<<PORTD2); // pull up
	EIMSK |= (1<<INT0); //enable external Interrupt 0 for Data ready Pin
	EICRA |= (1<<ISC01);//the falling edge generates the interrupt
	
	
	TWI_Master_Initialise();
	
	uint8_t sensorConfig[10];
	//(8-average, 15 Hz default, normal measurement)
	sensorConfig[0]=SLAVE_SENSOR_W;
	sensorConfig[1]=CRA;
	sensorConfig[2]=SENSOR_FREQUENCY;
	TWI_Start_Transceiver_With_Data(sensorConfig,3);
	vTaskDelay(100);
	sensorConfig[0]=SLAVE_SENSOR_W;
	sensorConfig[1]=CRB;
	sensorConfig[2]= SENSOR_GAIN; //(Gain=5, or any  other desired gain
	TWI_Start_Transceiver_With_Data(sensorConfig,3);
	vTaskDelay(100);
	sensorConfig[0]=SLAVE_SENSOR_W;
	sensorConfig[1]=MODE_REGISTER;
	sensorConfig[2]=MEASUREMENT_MODE; // single measurement mode or continous
	TWI_Start_Transceiver_With_Data(sensorConfig,3);
	vTaskDelay(100);
	#ifdef SEND_DEFINED_NUMBER
	uint16_t messagesSent=0;
	#endif
	


	PORTC &= ~(1<<PORTC1); // TURN OFF THE LED
	
	while(1){
		
		
			if (xSemaphoreTake(dataReadySemaphore,0xFFFF))//Block on sensor interrupt
			{
				
				sensorConfig[0]=SLAVE_SENSOR_R;
				sensorConfig[1]=DATA_OUTPUT_REGISTER; //Start with X MSB register
				TWI_Start_Transceiver_With_Data(sensorConfig,8); // 2 + 6 bytes to be read
				vTaskDelay(1);
				TWI_Get_Data_From_Transceiver(sensorConfig,8);// get the data from the i2c buffer into sensorconfig array
				
				for (uint8_t i =0;i<6;i++){
					sensorData[(messages*NUMBER_OF_BYTES_TO_READ)+i]=sensorConfig[i+2];
				}
				messages++;
				if (messages>=NUMBER_OF_MEASURES){
					#ifdef SEND_DEFINED_NUMBER
					messagesSent++;
					#endif
					
					messages=0;
					//sensorData[0]=0x10;
					pSensorData = sensorData;//send the messages in a queue to commTask
				    xQueueSend( DataToSend, (void*) &pSensorData, ( TickType_t ) 0 );
					
				}
				//we must setup the sensor for the next measurement in single measurement mode
				sensorConfig[0]=SLAVE_SENSOR_W;
				sensorConfig[1]=MODE_REGISTER;
				sensorConfig[2]=MEASUREMENT_MODE; // single measurement mode
				TWI_Start_Transceiver_With_Data(sensorConfig,3);
				vTaskDelay(1);
				#ifdef SEND_DEFINED_NUMBER
				//USED ONLY FOR TESTING PURPOSES, THIS WILL SUSPEND THE SENSOR TASK!
				if (messagesSent == NUMBER_OF_MESSAGES)
				{
					PORTC |= (1<<PORTC1); //TURN ON THE LED
					vTaskSuspend(xSensorHandle);
				}
				
				#endif
				
				
			}
			
			
		
		
		
		
		
	}
	
	
	
}

#else
/***************************

USE CONTINOUS MODE = TAKE MEASUREMENTS AT A GIVEN INTERVAL

****************************/



void xSensorTask (void* pvParameters){
	
	static uint8_t sensorData[NUMBER_OF_BYTES_TO_SEND];	uint8_t messages=0;	uint8_t *pSensorData;			DataToSend = xQueueCreate(1,sizeof(uint8_t*));	//only 1 element in queue, pointer to data
	dataReadySemaphore = xSemaphoreCreateBinary();
	//create semaphore for dataReady, the sensorTask will block on this semaphore
	while(xSemaphoreTake(moteReadySemaphore,0xFFFF) != pdTRUE);
	//wait a long time for mote to be ready to send
	
	TWI_Master_Initialise();
	
	uint8_t sensorConfig[10];
	//(8-average, 15 Hz default, normal measurement)
	sensorConfig[0]=SLAVE_SENSOR_W;
	sensorConfig[1]=CRA;
	sensorConfig[2]=0x70;//(8-average, 15 Hz default, normal measurement)
	TWI_Start_Transceiver_With_Data(sensorConfig,3);
	vTaskDelay(100);
	sensorConfig[0]=SLAVE_SENSOR_W;
	sensorConfig[1]=CRB;
	sensorConfig[2]= 0xA0; //(Gain=5, or any  other desired gain
	TWI_Start_Transceiver_With_Data(sensorConfig,3);
	vTaskDelay(100);
	sensorConfig[0]=SLAVE_SENSOR_W;
	sensorConfig[1]=MODE_REGISTER;
	sensorConfig[2]=0x00; // Continuous measurement mode
	TWI_Start_Transceiver_With_Data(sensorConfig,3);
	vTaskDelay(100);

	while(1){
		sensorConfig[0]=SLAVE_SENSOR_R;
		sensorConfig[1]=DATA_OUTPUT_REGISTER; //Start with X MSB register
		TWI_Start_Transceiver_With_Data(sensorConfig,8); // 2 + 6 bytes to be read
		vTaskDelay(1);
		TWI_Get_Data_From_Transceiver(sensorConfig,8);// get the data from the i2c buffer into sensorconfig array
		
		for (uint8_t i =0;i<6;i++){
			sensorData[(messages*NUMBER_OF_BYTES_TO_READ)+i]=sensorConfig[i+2];
		}
		messages++;
		if (messages>=NUMBER_OF_MEASURES){
			messages=0;
			sensorData[0]=0x10;
			pSensorData = sensorData;
			//xQueueSend( DataToSend, (void*) &pSensorData, ( TickType_t ) 0 );
			xQueueOverwrite(DataToSend, (void*) &pSensorData);
			//the queue is of length 1
			//overwrite if commTask is blocked because of no resources
		}
		vTaskDelay(10);

	}
}



#endif












