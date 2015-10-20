/*
 * LED.h
 *
 * Created: 10/8/2015 9:54:37 PM
 *  Author: Admin
 */ 

#ifndef LED_H_
#define LED_H_

#define PORT_LED_O PORTB
#define DDR_LED_O DDRB
#define BIT_LED_O 0

void vLEDInit(void);
void vLEDToggle(void);






#endif /* LED_H_ */
