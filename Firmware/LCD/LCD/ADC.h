#ifndef ADC_H_
#define ADC_H_

void adc_pin_config (void);
void read_adc(void);
void adc_config(void);
void update_adc_channel(char);
unsigned int avg_read_adc_channel(unsigned char, unsigned char);

#endif /* ADC_H_ */