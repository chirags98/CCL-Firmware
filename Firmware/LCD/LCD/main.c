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
#include "CCL.h"

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
		
	lcd_string2(1, 7, "CCL");
	_delay_ms(300);
	
	if (PINB & 0x20)
	{
		lcd_string2(2,1,"Supply Connected");
	}
	
	else
	{
		extern float offset;
		lcd_string2(2,2,"12v Supply NC");
		offset = 26.2;
	}
	
	_delay_ms(600);
	lcd_clear();
	
	while (1)
	{
		update_current();
		update_voltage();
		update_power();
		update_gate_voltage();
	
	if(check_thresholds()==0)	
	{	
		print_current(1,1);
		print_voltage(1,10);
		print_power(2,1);
		print_gate_voltage(2,11);
	}
	else
	{
		lcd_clear();
		char flag = 0;
		while(check_thresholds())
		{	
			update_current();
			update_voltage();
			update_power();
			update_gate_voltage();
			
			switch (check_thresholds())
			{
				case 1:	//Power exceeds threshold
					print_power(1,1);
					
					if(flag)
					break;
						
					lcd_string2(2, 1, "Power Exceeded");
					flag = 1;
					break;
			
				case 2:	//Voltage
					print_voltage(1,1);
					if(flag)
					break;
					
					lcd_string2(1, 1, "Voltage Exceeded");
					flag = 1;
					break;
			
				case 3:	//Current
					print_current(1,1);
					if(flag)
					break;
					
					lcd_string2(1, 1, "Current Exceeded");
					flag = 1;
					break;
			
				default:
					if(flag)
					break;
				
					lcd_string2(1, 6, "Error");
					flag = 1;
					break;
			}
		}
		lcd_clear();
	}
	}
}