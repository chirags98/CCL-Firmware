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
#include "lcd2.h"
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
	
	if (PINB & 0x20)	//Check if external 12v is connected
	{
		extern float offset;
		extern float fan_offset;
		lcd_string2(2,1,"Supply Connected");
		offset = 4;
		fan_offset = 0;
	}
	
	else				//Check if external 12v is NOT connected
	{
		extern float offset;
		lcd_string2(2,2,"12v Supply NC");
		offset = 31.2;
	}
	
	_delay_ms(600);
	lcd_clear();
	
	while (1)
	{
		update_current();
		update_voltage();
		update_power();
		//update_gate_voltage();
	
	if(check_thresholds()==0)		//If under safe limits print everything
	//if(1)	
	{	
		print_current(1,1);
		print_voltage(1,10);
		print_power(2,1);
		//print_gate_voltage(2,10);
		//print_mos_power(2,10);
		
		print_fan_status();
	}
	else							//If over safe limits print what is exceeded and show its value
	{		
		lcd_clear();
		
		/*
		To print what is exceeded only once,
		1 for power 
		2 for voltage 		
		3 for current
		4 for error
		*/
		char flag = 0;						
		
		while(check_thresholds())	//Stay here until over safe limits 
		{	
			update_current();
			update_voltage();
			update_power();
			update_gate_voltage();
			
			switch (check_thresholds())	//Switch according to what limit is exceeded
			{
				case 1:				//Power exceeds threshold
					print_power(1,1);					
					if(flag == 1)		//To print what is exceeded only once
					break;
						
					lcd_string2(2, 1, "Power Exceeded");
					flag = 1;		//What is exceeded is printed once
					break;
			
				case 2:	//Voltage exceeds threshold
					print_voltage(1,1);
					if(flag == 2)
					break;
					
					lcd_string2(2, 1, "Voltage Exceeded");
					flag = 2;
					break;
			 
				case 3:	//Current exceeds threshold
					print_current(1,1);
					if(flag == 3)
					break;
					
					lcd_string2(2, 1, "Current Exceeded");
					flag = 3;
					break;
			
				default:
					if(flag)
					break;
				
					lcd_string2(1, 6, "Error");
					flag = 4;
					break;
			}
		}
		lcd_clear();
	}
	}
}