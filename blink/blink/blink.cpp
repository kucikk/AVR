/*
 * blink.cpp
 *
 * Created: 4. 1. 2015 12:09:18
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */


#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	DDRB = 0xff;
	PORTB = 0xff;

	while(1) {
		PORTB = 0x00;
		_delay_ms(250);
		PORTB = 0xff;
		_delay_ms(250);
	}
}
