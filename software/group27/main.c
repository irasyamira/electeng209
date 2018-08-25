#include <avr/io.h>
#include <avr/interrupt.h>
#include "prototype.h"
#include <stdio.h>
#include <stdbool.h>

#define F_CPU 16000000UL
#include <util/delay.h>
#define BAUDRATE 9600

volatile uint8_t tot_overflow;
int numArray[4];

int main(void)
{
	float power = 0.0;								//initialize power variable
	float voltage = 0.0, current = 0.0;				//initialize V peak and I peak variables
	uint16_t BAUD = (F_CPU/BAUDRATE/16) - 1;		//set BAUD rate
	uart_init (BAUD);
	adc_init();										//initialize ADC
	DDRB |= (1 << 5);								//connect led to pin PC5
	
	timer0_init();									//initialize timer
	
	while(1)
	{

		//reading V peak from PC3
		voltage = adc_read(3);
		float Vpk = calculate_adc(voltage);
		float Vrms = (Vpk - 0.2)*(192/(sqrt(2)*12));

		//reading I peak from PC0
		current = adc_read(0);
		float Ipk = calculate_adc(current);
		float Irms = Ipk / (2*(sqrt(2))*0.33);
		
		power = calculate_power(Vrms,Irms);

		/* first 2s, tot_oveflow < 122, displays the voltage. 
		* the next 2s, 122 < tot_overflow < 244, displays the current
		* next 2s tot_overflow > 244, displays the calculated power */
		if (tot_overflow >= 244)				//approximately 4s
		{
			encode(power);
			for (int j = 0; j < 80; j++)
			{
				for (int k = 0 ; k<4 ; k++)
				{
					uart_transmit(numArray[k]);
					_delay_ms(5);
				}
			} 
			tot_overflow = 0;
		}
		else if (tot_overflow >= 122)			//approximately 2s
		{
			encode(Irms);
			for (int j = 0; j < 80; j++)
			{
				for (int k = 0 ; k<4 ; k++)
				{
					uart_transmit(numArray[k]);
					_delay_ms(5);
				}
			}
			tot_overflow = 244;
		}		
		else
		{
			encode(Vrms);						//this will display the Vrms
			for (int j = 0; j < 80; j++)
			{
				for (int k = 0 ; k<4 ; k++)
				{
					uart_transmit(numArray[k]);
					_delay_ms(5);
				}
			}
			tot_overflow = 122;
		}

		/*LED blinks every 1s when 25%<power
		* blinks every 0.50s when 25%<power<50%
		* blinks every 0.33s when 50%<power<75%
		* constantly on when 75%<power<100% */
		if (power < 2.125) {				//power < 25%
			//delay 1s
			if (tot_overflow >= 61)
			{
				if (TCNT0 >= 250)
				{
					PORTB ^= (1 << 5);		//toggles the led every 1s
					TCNT0 = 0;				//reset counter
					tot_overflow = 0;		//reset overflow counter
				}
			}
		} else if ((power < 4.25) & (power >= 2.125)) { //20% < power < 50%
			//delay 0.5s
			if (tot_overflow >= 31)
			{
				if (TCNT0 >= 250)
				{
					PORTB ^= (1 << 5);		//toggles the led every 0.5s
					TCNT0 = 0;				//reset counter
					tot_overflow = 0;		//reset overflow counter
				}
			}
		} else if ((power < 6.38) & (power >= 4.25)) { //20% < power < 50%
			//delay 0.333s
			if (tot_overflow >= 15)
			{
				if (TCNT0 >= 250)
				{
					PORTB ^= (1 << 5);    //toggles the led every 0.33s
					TCNT0 = 0;            //reset counter
					tot_overflow = 0;     //reset overflow counter
				}
			}
		} else { //(power >= 6.38)
			PORTB |= (1 << 5);    //keeps the led on
			//no delay
		}
	}
}


		