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

volatile int i =0;

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
	
	float voltage = 0;
	float current = 0;
	float power = 0;
	float gate_voltage = 0;
	float offset = 0;
	
	lcd_string2(1, 7, "CCL");
	_delay_ms(300);
	
	if (PINB & 0x20)
	{
		lcd_string2(2,1,"Supply Connected");
	}
	
	else
	{
		lcd_string2(2,2,"12v Supply NC");
		offset = 28.2;
	}
	
	_delay_ms(600);
	lcd_clear();
	
	while (1)
	{
		current = avg_read_adc_channel(0, 10);
		current = current*1.953+5.7 + offset;
		lcd_print(1,1,current,4);
		lcd_string2(1,5,"mA  ");
				
		voltage = avg_read_adc_channel(1, 10);
		voltage = voltage*29.3;
		lcd_print(1,10,voltage,5);
		lcd_string2(1,15,"mV");
		
		power = voltage*current/1000;
		lcd_print(2,1,power,5);
		lcd_string2(2,6,"mW");
		
		/*
		gate_voltage = avg_read_adc_channel(2, 5)*7.5;
		lcd_print(2,11,gate_voltage,4);
		lcd_string2(2,15,"mV");
		*/
				
		_delay_ms(10);
		//i++;
		lcd_print(2,11,i,3);
	}
}