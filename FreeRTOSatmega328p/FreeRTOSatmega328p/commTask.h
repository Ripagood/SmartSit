/*
 * commTask.h
 *
 * Created: 10/9/2015 12:57:32 AM
 *  Author: Admin
 */ 


#ifndef COMMTASK_H_
#define COMMTASK_H_

void startCommTasks(UBaseType_t uxPriority);
void xMoteTask( void* pvParameters);
#endif /* COMMTASK_H_ */
