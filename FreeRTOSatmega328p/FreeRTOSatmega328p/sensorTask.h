/*
 * sensorTask.h
 *
 * Created: 10/9/2015 11:00:08 PM
 *  Author: Admin
 */ 


#ifndef SENSORTASK_H_
#define SENSORTASK_H_

void startSensorTasks(UBaseType_t uxPriority);
void xSensorTask( void* pvParameters);
#endif /* SENSORTASK_H_ */
