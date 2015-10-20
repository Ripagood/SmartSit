/*
 * mytasks.c
 *
 * Created: 10/8/2015 10:00:59 PM
 *  Author: Admin
 */ 


#include "FreeRTOS.h"#include "task.h"
#include "Drivers/LED.h"
#include "mytasks.h"
void vLEDFlashTask( void *pvParameters ){
	vLEDInit();
	portTickType xLastWakeTime;
	const portTickType xFrequency = 1000;
	xLastWakeTime=xTaskGetTickCount();
	for( ;; )	{
		vLEDToggle();
		vTaskDelayUntil(&xLastWakeTime,xFrequency);}
}



