/*
 * LED.c
 *
 * Created: 10/8/2015 9:54:27 PM
 *  Author: Admin
 */ 
#include <avr/io.h>
#include "LED.h"
void vLEDInit(void)
{
	// Set LED_O as output pin
	//DDR_LED_O |= (1<<BIT_LED_O);	DDR_LED_O |= 0xFF;
}
void vLEDToggle(void)
{
	//Toggle LED
	PORT_LED_O ^= (1<<BIT_LED_O);
}