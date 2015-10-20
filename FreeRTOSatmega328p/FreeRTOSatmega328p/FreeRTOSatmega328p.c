/*
 * FreeRTOSatmega328p.c
 *
 * Created: 10/8/2015 9:33:00 PM
 *  Author: Admin
 */ 

#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "mytasks.h"#include "commTask.h"#include "sensorTask.h"#include "queue.h"

/* Priority definitions for most of the tasks in the demo application.  Some
tasks just use the idle priority. */
#define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY +1)#define mainComm_TASK_PRIORITY			( tskIDLE_PRIORITY +2 )#define mainSensor_TASK_PRIORITY		( tskIDLE_PRIORITY +2 )

portSHORT main(void)
{	startCommTasks(mainComm_TASK_PRIORITY);
    startSensorTasks( mainSensor_TASK_PRIORITY);
	
	xTaskCreate( vLEDFlashTask, ( signed char * ) "LED", configMINIMAL_STACK_SIZE, NULL, mainLED_TASK_PRIORITY, NULL );
	//start scheduler
	vTaskStartScheduler();
	//you should never get here
	while(1){	}
	return 0;
}