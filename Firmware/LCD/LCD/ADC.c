/*
 * ADC.c
 *
 * Created: 22-Jul-17 11:11:48 PM
 *  Author: Chirag
 */ 

#include <avr/io.h>
#include "LCD.h"

//ADC pin configuration
void adc_pin_config (void)
{
	DDRC = 0x00; //set PORTF direction as input
	PORTC = 0x00; //set PORTF pins floating
}

void adc_config(void)
{
	ADCSRA=0x85; //1000 01 01 ADC Enable, ADC Interrupt Enable, ADC Prescaler=2
}

unsigned int read_adc()
{
	ADCSRA|=0x40; //ADSC: ADC Start Conversion: In Single Conversion mode, write this bit to one to start each conversion.
	while((ADCSRA & 0x10)==0); //ADIF: ADC Interrupt Flag: This bit is set when an ADC conversion completes and the Data Registers are updated.
	ADCSRA|=0x10;	//ADIF is cleared by writing a logical one to the flag.
	
	unsigned char low = ADCL;
	unsigned char high = ADCH;
	
	unsigned int val = 0x03 && high;
	val = val<<8;
	val = 256*high + low;
	
	return val;
}