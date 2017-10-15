/*
 * CCL.c
 *
 * Created: 07-Oct-17 12:01:48 PM
 *  Author: Chirag
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "main.h"
#include "lcd.h"
#include "ADC.h"
#include "CCL.h"

float voltage = 0;
float current = 0;
float power = 0;
float gate_voltage = 0;
float offset = 0;

int current_thresh = 1500;
int v_thresh = 25000;
int power_thresh = 10000;

void update_current()
{
	current = avg_read_adc_channel(0, 10);		//0-2A	Least count = 1.95mv
	current = current*1.953 + 0.3 + offset;		//1.953*(1.0228, 9.57, 1.1054, 1.016,1.0570)
}

void update_voltage()
{
	voltage = avg_read_adc_channel(1, 10);		//0-30V	Least count = 29.29mv
	voltage = voltage*29.29;	
}

void update_power()
{
	power = voltage*current/1000;
}

void update_gate_voltage()
{
	gate_voltage = avg_read_adc_channel(2, 10)*7.5;		//0-7.68V	
}

void print_current(char row, char col)
{
	lcd_print2(row,col,current,4, "mA");					//Least count = 1.95mA
	//lcd_string2(row,col+4,"mA  ");
}

void print_voltage(char row, char col)
{
	lcd_print2(row,col,voltage,5, "mV");					//Least count = 29.29mv
	//lcd_string2(row,col+5,"mV");
}

void print_power(char row, char col)
{
	lcd_print2(row,col,power,5, "mW");
	//lcd_string2(row,col+5,"mW");
}

void print_gate_voltage(char row, char col)
{
	lcd_print2(row,col,gate_voltage,4, "mV");				//Least count = 7.5mv
	//lcd_string2(row,col+4,"mV");
}

char check_thresholds()
{
	static char val = 0;
	
	if (current<current_thresh && voltage<v_thresh && power<power_thresh)
	{
		if (val == 1)
		{
			power_thresh += 500;
			v_thresh += 2000;
			current_thresh += 100;
			val = 0;
		}
		
		return 0;	//All okay
	}
	
	else
	{	
		if (val == 0)
		{
			power_thresh -= 500;
			v_thresh -= 2000;
			current_thresh -= 100;
			val = 1;
		}
		
		if (power>power_thresh)
			return 1;
	
		else if (voltage>v_thresh)
			return 2;

		else if (current>current_thresh)	
			return 3;
	
		else
			return 4;	//Error
	}
}