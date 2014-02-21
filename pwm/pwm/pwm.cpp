/*
 * pwm.cpp
 *
 * Created: 26. 1. 2014 18:02:08
 *  Author: Tomáš
 */ 

#define F_CLK 1000000
#define F_CPU 1000000

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


uint32_t pulse_time = 0;


int main(void)
{
	
	DDRB = 0;//nastaví port B jako vstup
	PORTB = 0;//vypne pullup rezistory
	
	TCCR1B |= (1 << ICES1);//nastaví capture na "rising" hranu
	TCCR1B = (TCCR1B & 0xf8) | 2;// /8 prescaling
	TIMSK |= (1 << TICIE1);
	
	sei();//zapne přerušení
	
    while(1)
    {
		printf("pulse time: %u us   ", pulse_time);
		printf("pos: %u %%          ", (pulse_time - 1000) / 10);
		_delay_ms(100);
    }
	
	return 1;
}


ISR(TIMER1_CAPT_vect)
{
	static int8_t rising;
	static uint32_t start_time;
	
	rising = !rising;
	if (rising) {
		start_time = ICR1;
		TCCR1B &= ~(1 << ICES1);//přepne zachytávání na "falling edge"
	} else {
		pulse_time = (ICR1 - start_time) * 8;// *8 vyruší /8 prescaling
		TCCR1B |= (1 << ICES1);
		TCNT1 = 0;
	}
}
