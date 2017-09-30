/*
 * LCD.c
 *
 * Created: 5/20/2017 1:57:55 PM
 * Author : Chirag
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "main.h"
#include "lcd.h"
#include "ADC.h"

void port_config (void)
{
	lcd_port_config();
	adc_pin_config();
}

void init_devices (void)
{
	cli(); //Clears the global interrupts
	lcd_start();
	adc_config();
	sei();   //Enables the global interrupts
}

int main(void)
{
	port_config();
	init_devices();
	
	ADMUX=0x41;    //Channel 1
	int i = 0;
	while (1)
	{
		//unsigned int val = read_adc();
		//val = val*5/1023*1000;
		//lcd_print(1,1,val,5);
		lcd_print(2,1,i,3);
		i++;
		_delay_ms(1000);
	}
}