#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "prototype.h"
volatile uint8_t tot_overflow;
volatile float power;	
uint16_t adc_value;	//ADC result must be read into a global variable
float Vref = 5.0;
float stepsize;
int numArray[4];
//volatile float power = 0.0;	

/*enables and initializes the UART*/
void uart_init(uint16_t MYUBRR)
{
	UBRR0H = (MYUBRR>>8);				//Set baud rate using register UBRR0H and UBRR0L
	UBRR0L = MYUBRR;
	UCSR0B |= (1<<TXEN0);				//Enable transmitter
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01);	//Select 8-bit frame, single stop bit and no parity	
}

/*transmits the packet of data encoded to the CPLD*/
void uart_transmit(uint8_t data)
{
	while (!(1<<UDRE0) && (UCSR0A));	//checking the registers
	UDR0 = data;

}

/*enables and initializes the ADC with pre scaler of 64, running on single
* capture mode with interrupt enables, result right justified*/
void adc_init( )
{

	/** Setup and enable ADC **/
	ADMUX |=	(0<<REFS1)|				// Reference Selection Bits
				(1<<REFS0)|				// AVcc - external cap at AREF
				(0<<ADLAR)|				// ADC Left Adjust Result
				(0<<MUX2)|				// Analog Channel Selection Bits
				(0<<MUX1)|				// ADC2 (PC2 PIN25)
				(0<<MUX0);
	
	ADCSRA =	(1<<ADEN)|				// ADC Enable
				(1<<ADSC)|				// ADC Start Conversion
				(0<<ADATE)|				// ADC Auto Trigger Enable
				(0<<ADIF)|				// ADC Interrupt Flag
				(0<<ADIE)|				// ADC Interrupt Enable - interrupt disabled
				(1<<ADPS2)|				// ADC Pre scaler Select Bits //F_ADC = F_CPU/64
				(1<<ADPS1)|
				(0<<ADPS0);
}

/*encodes a float num to its respective 4 numbers in decimal to be transmitted
* to the CPLD*/
void encode(float num)
{
	//to flag for decimal point
	int dec1 = 0;
	int dec2 = 0;
	int dec3 = 0;						
	int num1, num2, num3, num4;

	if ((num/10.0) < 1) {
		dec1 = 1;
		num = num*1000;
	} else if ((num/10.0) < 10) {
	dec2 = 1;
		num = num*100;
	} else if ((num/10.0) < 100) {
		dec3 = 1;
		num = num*10;
	} else {							//((num/10.0) < 1000)
		num = num*1;
	}

	int intNum = (int)num;				//change num from float to int
	
	/*start modulus calculation to hard code and transmit by breaking down the 4digit number into
	* separate digits with decimal point assigned (if applicable)*/
	num1 = intNum/1000;					
	if (dec1 == 1) {
		numArray[0] = (num1 + 112);		//store values in an array of 4 elements for 4 digits
		} else {
		numArray[0] = (num1 + 96);
	}
	intNum = intNum%1000;
	num2 = intNum/100;
	if (dec2 == 1) {
		numArray[1] = (num2 + 80);
		} else {
		numArray[1] = (num2 + 64);
	}
	intNum = intNum%100;
	num3 = intNum/10;
	if (dec3 == 1) {
		numArray[2] = (num3 + 48);
		} else {
		numArray[2] = (num3 + 32);
	}
	num4 = intNum%10;
	numArray[3] = (num4);

}

/*reads the value at the specified channel given that the PORT of interest
* has been initialized to input*/
int adc_read(uint8_t channel)
{
	uint16_t adc_value;

	ADMUX	&= 0x00;					//clears the channel that was previously read
	ADMUX	|= channel;					//set to new ADC channel to be read (from function input)
	ADMUX	|=	(1<<REFS0);				//reset the Vref because has been cleared in previous channel assignment
	ADCSRA	|=	(1 << ADSC);			//starts a new conversion
	while (ADCSRA & (1 << ADSC));		//wait for completion of ADC conversion
	adc_value = ADC;					//assign variable to pass ADC value
	return (adc_value);					//returns the value from the ADC channel 0

}

/*calculates the input voltage value applied at ADC0 channel by multiplying it
* with step size, and stores the result in a variable of type float or double*/
float calculate_adc(uint16_t adc_value)
{
	float Vin;
	stepsize = Vref/1024.0;
	Vin = adc_value*stepsize;
	Vin = (adc_value * Vref)/1024.0;
	return Vin;
}

/*multiplies the values of V and I in the two arrays and storing the calculated values
* in a power array to be passed to another function which will be calculating its average value*/
float calculate_power(float v, float i)
{
	return v*i;
}

/*calculates the average of the values contained in an array, provided that the values
* in the array are of float type*/
//function not used because no sampling is done
float calculate_avg(float array[])
{
	float sum = 0;						//this is to sum all the values in the array
	int j;								//index number for the array
	int x = 19;							//x = length of array
	float average;

	for (j=0; j<x; j++)
	{
		sum = sum + array[j];			//total of all the values in the array
	}

	average = sum/x;					//float divide by int is float
	return(average);
}

/*enables and initializes timer0 with pre scaler of 1024, initializing counter to 0
* with enabled overflow interrupt, enabled global interrupt, and initialized
* overflow counter variable*/
void timer0_init()
{
	TCCR0B |= (1 << CS02) | (1<<CS00);	//set up timer with pre scaler = 1024
	TCNT0 = 0;							//initialize counter
	TIMSK0 |= (1 << TOIE0);				//enable overflow interrupt
	sei();								//enable global interrupts
	tot_overflow = 0;					//initialize overflow counter variable
}

/*this Interrupt Service Routine keeps track of the number of overflows
* when timer0 is set to count up to more than 16.32ms*/
ISR(TIMER0_OVF_vect)
{
	tot_overflow++;
}
