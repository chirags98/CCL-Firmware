/*
 * CCL.h
 *
 * Created: 07-Oct-17 12:02:06 PM
 *  Author: Chirag
 */ 


#ifndef CCL_H_
#define CCL_H_

void update_current(void);
void update_voltage(void);
void update_power(void);
void update_gate_voltage(void);

void print_voltage(char, char);
void print_current(char, char);
void print_power(char, char);
void print_gate_voltage(char, char);

char check_thresholds();

#endif /* CCL_H_ */