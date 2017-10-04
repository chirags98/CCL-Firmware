/*
 * ADC.c
 *
 * Created: 22-Jul-17 11:11:48 PM
 *  Author: Chirag
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include "LCD.h"
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <avr/sleep.h>

volatile unsigned int adc_val[3];
volatile char flag = 0;

extern volatile int i;

//ADC pin configuration
void adc_pin_config (void)
{
	DDRC = 0x00; //set PORTF direction as input
	PORTC = 0x00; //set PORTF pins floating
}

void adc_config(void)
{
	//ADCSRA=0x87;	//1 --- 0 111 ADC Enable, ADC Interrupt disable, ADC Prescaler=128
	ADCSRA=0x8F;	//1 --- 1 111 ADC Enable, ADC Interrupt enable, ADC Prescaler=128
	SMCR |= 0x02;	//---- 001 0 ADC Noise Reduction mode, sleep enable/disable
}

void read_adc()
{
	//ADCSRA|=0x40; //ADSC: ADC Start Conversion: In Single Conversion mode, write this bit to one to start each conversion.
	//while((ADCSRA & 0x10)==0); //ADIF: ADC Interrupt Flag: This bit is set when an ADC conversion completes and the Data Registers are updated.
	//ADCSRA|=0x10;	//ADIF is cleared by writing a logical one to the flag.
		
	//flag = 1;
	SMCR |= 0x01;		//ADC Start Conversion
	//while(flag==1);		//ADIF: ADC Interrupt Flag: This bit is set when an ADC conversion completes and the Data Registers are updated.
	//SMCR &= 0x02;
}

void update_adc_channel(char channel)
{
	if (channel>=0 && channel<=7)
	{	
		ADMUX = 0x00 | channel;	//00 - - AREF, Internal Vref turned off :--: 0000  ADC 0 (Channel)
		_delay_ms(1);
		read_adc();
	}
}

unsigned int avg_read_adc_channel(unsigned char channel, unsigned char i)
{
	float val = 0;
	
	for(int j = 1;j<=i;j++)
	{
		update_adc_channel(channel);
		val = val + adc_val[channel]/i;
	}
	
	adc_val[channel] = val;
	
	return adc_val[channel];
}

//Conversion complete 
ISR(ADC_vect)
{
	unsigned char channel = ADMUX & 0x0F;	//Read channel number by masking upper nibble
	
	//ADIF is automatically cleared by writing a logical one to the flag.
	ADCSRA|=0x10;	//ADIF is cleared by writing a logical one to the flag.
	//Read ADC value
	volatile unsigned char low = ADCL;
	volatile unsigned char high = ADCH;
	
	adc_val[channel] = 0x03 && high;
	adc_val[channel] = adc_val[channel]<<8;
	adc_val[channel] = 256*high + low;
	//adc_val[channel] = 500;
	
	i++;
	flag=0;		//conversion complete
}