/*
 * stepper_motor.cpp
 *
 * Created: 13. 9. 2014 17:58:58
 *  Author: Tomáš Kučera
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/iotn2313.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t state = 0;
uint16_t anti_speed = 0x00ff;
uint8_t seq[] = {
	0x08,
	0x0c,
	0x04,
	0x06,
	0x02,
	0x03,
	0x01,
	0x09
};

void rotateCW();
void rotateCCW();
void delay_ms(uint32_t);
void blink();
void blink(uint32_t);
void blink(uint32_t, uint32_t);

int main(void) {
	//uint8_t seq[] = {0x08, 0x04, 0x02, 0x01};
	uint8_t ccw = 1;
		
	// reset A
 	DDRA = 0x00;
	PORTA = 0x00;
	// reset B
	DDRB = 0x00;
	PORTB = 0x00;
	// reset C
	DDRD = 0x00;
	PORTD = 0x00;
	

	DDRB = 0xff;//B as OUT
	PORTB = 0x00;//set to LO

	DDRD = 0x00;//D as IN
	PORTD = 0xff;//enable pull-up
	
	PORTB |= (1 << PB7);
	
	blink(200, 400);
	
    while(1) {
		if (~PIND & (1 << PD2)) {
			PORTB |= 1 << PB6;
			delay_ms(200);
			PORTB &= ~(1 << PB6);
			if (~PIND & (1 << PD2)) {
				ccw = !ccw;
				blink(125, 500);
			}
		}
		PORTB &= ~(1 << PB6);
		if (ccw) {
			rotateCCW();
		} else {
			rotateCW();
		}
    }
}

void delay_ms (uint32_t duration) {
	while (duration--) {
		//_delay_ms(1);
	}
}

void blink (uint32_t delay, uint32_t duration) {
	if (!delay) delay = 1;
	for (uint8_t i = 0; i < duration / delay; i++) {
		PORTB |= 1 << PB6;
		delay_ms(delay);
		PORTB &= ~(1 << PB6);
		delay_ms(delay);
	}
}

void rotateCW () {
	PORTB |= 1 << PB7;
	uint8_t tmp, i;
	for (i = 0; i < 8; i++) {
		tmp = (PORTB & 0xf0) | (seq[i] & 0x0f);
		PORTB = tmp;
		delay_ms(anti_speed);
	}
}

void rotateCCW () {
	PORTB &= ~(1 << PB7);
	uint8_t tmp, i;
	for (i = 7; i >= 0; i--) {
		tmp = (PORTB & 0xf0) | (seq[i] & 0x0f);
		PORTB = tmp;
		delay_ms(anti_speed);
	}
}
