#ifndef PROTOTYPE_H_
#define PROTOTYPE_H_
void uart_init(uint16_t MYUBRR);
void uart_transmit(uint8_t data);
void adc_init( );
void encode(float num);
int adc_read(uint8_t channel);
float calculate_adc(uint16_t adc_value);
float calculate_power(float v, float i);
float calculate_avg(float array[]);
void timer0_init();
ISR(TIMER0_OVF_vect);
#endif /* PROTOTYPE_H_ */