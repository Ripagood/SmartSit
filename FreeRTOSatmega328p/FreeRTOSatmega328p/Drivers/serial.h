/*
 * serial.h
 *
 * Created: 10/8/2015 11:32:26 PM
 *  Author: Admin
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_
typedef void * xComPortHandle;
xComPortHandle xSerialPortInitMinimal( unsigned portBASE_TYPE uxQueueLength );
signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime );
signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime );
uint8_t sendMoteData(uint8_t* data, uint8_t data_size);
int8_t serialMoteReceive(void);
void serialMoteSend(int8_t tx);
void configureMote(void);
#endif /* SERIAL_H_ */