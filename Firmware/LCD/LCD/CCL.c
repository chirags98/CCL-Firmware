/*
 * CCL.c
 *
 * Created: 07-Oct-17 12:01:48 PM
 * Author: Chirag
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

float voltage = 0;
float current = 0;
float power = 0;
float power_mos = 0;
float gate_voltage = 0;
float offset = 0;

int current_thresh = 2000;
int v_thresh = 30000;
int power_thresh = 20000;
int fan_power_thresh = 5000;

void update_current()
{
	current = avg_read_adc_channel(0, 5);		//0-2A	Least count = 1.95mv
	current = current*1.953 + 0.3 + offset;		//1.953*(1.0228, 9.57, 1.1054, 1.016,1.0570)
}

void update_voltage()
{
	voltage = avg_read_adc_channel(1, 5);		//0-30V	Least count = 29.29mv
	voltage = voltage*29.29;					//0-30000
	voltage = voltage - (int)voltage%10;				//Round down the last digit for correct resolution
}

void update_power()
{
	power = voltage*current/1000;
	power = power - (int)power%10;
	
	power_mos = power - current*current/1000;
	power_mos = power_mos - (int)power_mos%10;
}

void update_gate_voltage()
{
	gate_voltage = avg_read_adc_channel(2, 10)*10.74;		//0-11V	//10.74mV resolution
	gate_voltage = gate_voltage - (int)gate_voltage%10;			//Round down the last digit for correct resolution
}

void print_current(char row, char col)
{
	lcd_print4(row, col, current, "mA", "A");
}

void print_voltage(char row, char col)
{
	lcd_print4(row, col, voltage, "mV", "V");
}

void print_power(char row, char col)
{
	lcd_print4(row, col, power, "mW", "W");
}

void print_gate_voltage(char row, char col)
{
	lcd_print4(row, col, gate_voltage, "mV", "V");
}

void print_mos_power(char row, char col)
{
	lcd_print4(row, col, power_mos, "mW", "W");
}

void print_fan_status(void)
{
	static char val = 0;	//To create a schmitt trigger like mechanism for power, voltage and current.
	
	if (power_mos > fan_power_thresh)
	{
		if (val == 0)	//If values are set at the nominal value decrease the values
		{
			fan_power_thresh -= 500;
			val = 1;
		}
		
		lcd_string2(2,10, "Fan On ");
	}
	
	else
	{
		if (val == 1)	//If levels during last check was more than threshold - reset threshold to nominal values
		{
			fan_power_thresh += 500;
			val = 0;
		}
		
		lcd_string2(2,10, "Fan Off");
	}	
}

char check_thresholds()
{
	static char val = 0;	//To create a schmitt trigger like mechanism for power, voltage and current.
	
	//If less than threshold
	if (current<current_thresh && voltage<v_thresh && power_mos<power_thresh)
	{
		if (val == 1)	//If levels during last check was more than threshold - reset threshold to nominal values
		{
			power_thresh += 500;
			v_thresh += 500;
			current_thresh += 100;
			val = 0;	//Values have been reset
		}
		
		return 0;	//All okay
	}
	
	//If greater than threshold
	else
	{	
		if (val == 0)	//If values are set at the nominal value decrease the values
		{
			power_thresh -= 500;
			v_thresh -= 500;
			current_thresh -= 100;
			val = 1;	//Values have been decreased
		}
		
		if (power_mos>power_thresh)
			return 1;
	
		else if (voltage>v_thresh)
			return 2;

		else if (current>current_thresh)	
			return 3;
	
		else
			return 4;	//Error
	}
}