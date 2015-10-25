/*
 * SmartSit.h
 *
 * Created: 10/10/2015 6:24:28 PM
 *  Author: Admin
 */ 


#ifndef SMARTSIT_H_
#define SMARTSIT_H_
/*********************/
/*   SENSOR DEFINES  */
/*********************/

/*used only for testing purposes only. If defined, the sensorTask will suspend 
when reaching the number of messages sent to the manager reaches NUMBER_OF_MESSAGES*/
#define SEND_DEFINED_NUMBER 
#ifdef SEND_DEFINED_NUMBER
	#define NUMBER_OF_MESSAGES 5000 //how many messages to send
#endif

#define DATA_READY_INTERRUPT
//comment DATA_READY_INTERRUPT for continous sensor mode


#define NUMBER_OF_MEASURES			8  
// how many X,Y,Z measures to take from the sensor
#define NUMBER_OF_AXIS				3
#define SIZE_OF_AXIS				2
// each axis is of size 2 bytes
#define NUMBER_OF_BYTES_TO_READ		NUMBER_OF_AXIS*SIZE_OF_AXIS

#define NUMBER_OF_BYTES_TO_SEND		NUMBER_OF_MEASURES*NUMBER_OF_AXIS*SIZE_OF_AXIS

#define TICK_RATE_HZ				1000
#define SENSOR_FREQUENCY_HZ			15
#define SENSOR_PERIOD_TICKS			66

/*********************/
/*   MOTE DEFINES    */
/*********************/
#define serviceID 0x03  //Service ID for the Mote, select a different one for each mote and micro, starting at 1
#define MOTE_TIME 300ul // time in ms
#define MOTE_TIME_LOW (uint8_t)(MOTE_TIME)
#define MOTE_TIME_HIGH (uint8_t)(MOTE_TIME>>8)
#define BUFFER_LENGHT 64
#define HDLC_OUTPUT_BUFFER_LENGTH 82
#define HDLC_DECODED_BUFFER__LENGTH 32
#define RX_BUFFER_LENGTH 24


#endif /* SMARTSIT_H_ */