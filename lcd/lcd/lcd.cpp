/*
 * lcd.cpp
 *
 * Created: 22. 11. 2014 18:41:34
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */

#define F_CPU 1000000UL//1MHz - ATMega8 default
#define SCL_CLOCK 50000UL//I2C clock 50kHz

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "twi/twi.h"

#define DEV_ADDR 0b00100111

#define ERROR 1
#define SUCCESS 0

#define STATUS_LED1 PB0
#define STATUS_LED2 PB1
#define STATUS_LED3 PB2



/**
 * WAIT - in milliseconds
 */
void wait_ms (uint32_t delay) {
	while (delay >= 1000) {
		delay -= 1000;
		_delay_ms(1000);
	}
	while (delay >= 100) {
		delay -= 100;
		_delay_ms(100);
	}
	while (delay >= 10) {
		delay -= 10;
		_delay_ms(10);
	}
	while (delay >= 1) {
		delay -= 1;
		_delay_ms(1);
	}
}



/**
 * BLINK
 */
void blink (uint8_t count, uint32_t delay) {
	wait_ms(delay);
	while (count--) {
		PORTB &= ~(1 << STATUS_LED1);
		wait_ms(delay);
		PORTB |= 1 << STATUS_LED1;
		wait_ms(delay);
	}
	PORTB &= ~(1 << STATUS_LED1);
}

/**
 * BLINK - 500ms delay
 */
void blink (uint8_t count) {
	blink(count, 200);
}

/**
 * BLINK - 3 times, 100ms delay
 */
void blink () {
	blink(3, 200);
}



/**
 * BLINK2
 */
void blink2 (uint8_t count, uint32_t delay) {
	wait_ms(delay);
	while (count--) {
		PORTB &= ~(1 << STATUS_LED2);
		wait_ms(delay);
		PORTB |= 1 << STATUS_LED2;
		wait_ms(delay);
	}
	PORTB &= ~(1 << STATUS_LED2);
}

/**
 * BLINK2 - 500ms delay
 */
void blink2 (uint8_t count) {
	blink2(count, 200);
}

/**
 * BLINK2 - 3 times, 100ms delay
 */
void blink2 () {
	blink2(3, 200);
}



/**
 * BLINK3
 */
void blink3 (uint8_t count, uint32_t delay) {
	wait_ms(delay);
	while (count--) {
		PORTB &= ~(1 << STATUS_LED3);
		wait_ms(delay);
		PORTB |= 1 << STATUS_LED3;
		wait_ms(delay);
	}
	PORTB &= ~(1 << STATUS_LED3);
}

/**
 * BLINK3 - 500ms delay
 */
void blink3 (uint8_t count) {
	blink3(count, 100);
}

/**
 * BLINK3 - 3 times, 100ms delay
 */
void blink3 () {
	blink3(3, 100);
}


/**
 * Přepne 3 stavovou ledku
 */
void toggleLed3 () {
	if (PORTB & 1 << STATUS_LED3) {
		PORTB &= ~(1 << STATUS_LED3);
		} else {
		PORTB |= 1 << STATUS_LED3;
	}
}





uint8_t LCDSendNibble(TWI twi, bool rs, bool rw, uint8_t nibble) {
	
	uint8_t byte = 0x0f & nibble;
	if (rw) byte |= 1 << 6;
	if (rs) byte |= 1 << 7;
	
	twi.write(byte);//(RS, R/W, E=0, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) return 1;
	
	twi.write(byte | (1 << 5));//(RS, R/W, E=1, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) return 2;
	
	return 0;
}


uint8_t LCDSendByte(TWI twi, bool rs, bool rw, uint8_t byte) {
	
	if (LCDSendNibble(twi, rs, rw, byte >> 4)) return 1;
	
	if (LCDSendNibble(twi, rs, rw, byte & 0x0f)) return 2;
	
	return 0;
}


uint8_t LCDInit(TWI twi) {
	
	twi.start();
	if (!twi.statusIs(TW_START)) return 1;

	twi.write(DEV_ADDR << 1);// pošle SLA+W (slave address + write bit = poslední bit je nula)
	if (!twi.statusIs(TW_MT_SLA_ACK)) return 2;
	
	return 0;
}


uint8_t LCDSetup(TWI twi) {
	
	blink();
	
	toggleLed3();
	// nastaví 4-bitovou komunikaci
	if (LCDSendNibble(twi, 0, 0, 0b0010))//0, 0, 1, 4bit
		return 1;
	wait_ms(500);
	
	blink();
	
	toggleLed3();
	// potvrdí 4 bitovou komunikaci a nastaví vlastnosti displeje
	if (LCDSendByte(twi, 0, 0, 0b00101000))//0, 0, 1, 4bit, 2 lines, font 5x8,-,-
		return 2;
	wait_ms(500);
	
	toggleLed3();
	// zapne displej, kurzor, blikání kurzoru
	if (LCDSendByte(twi, 0, 0, 0b00001111))//0, 0, 0, 0, 1, display on, cursor on, blinking cursor
		return 3;
	wait_ms(500);
	
	toggleLed3();
	// zastaví posuv kurzoru
	if (LCDSendByte(twi, 0, 0, 0b00000110))//0, 0, 0, 0, 0, 1, increment/decrement, scroll display while writing
		return 4;
	wait_ms(500);
		
	return 0;
}


uint8_t LCDWrite(TWI twi) {
	
	blink();
	
	toggleLed3();
	// napíše H
	if (LCDSendByte(twi, 1, 0, 0b01001000))
		return 1;
	wait_ms(500);
	
	toggleLed3();
	// napíše I
	if (LCDSendByte(twi, 1, 0, 0b01001001))
		return 2;
	wait_ms(500);
	
	toggleLed3();
	// napíše H
	if (LCDSendByte(twi, 1, 0, 0b01001000))
		return 3;
	wait_ms(500);
	
	toggleLed3();
	// napíše I
	if (LCDSendByte(twi, 1, 0, 0b01001001))
		return 4;
	wait_ms(500);
	
	toggleLed3();
	// napíše H
	if (LCDSendByte(twi, 1, 0, 0b01001000))
		return 5;
	wait_ms(500);
	
	toggleLed3();
	// napíše I
	if (LCDSendByte(twi, 1, 0, 0b01001001))
		return 6;
	wait_ms(500);
	
	return 0;
}



/**
 * Úvodní zablikání ledkami
 */
void LEDInit (void) {
	PORTB &= 0xf8;
	wait_ms(250);
	PORTB |= 1 << STATUS_LED1;
	wait_ms(250);
	PORTB |= 1 << STATUS_LED2;
	wait_ms(250);
	PORTB |= 1 << STATUS_LED3;
	wait_ms(250);
	
	// osvětlit
	PORTB |= (1 << STATUS_LED1) | (1 << STATUS_LED2) | (1 << STATUS_LED3);
	wait_ms(500);
	
	// pohasnout
	PORTB &= ~((1 << STATUS_LED1) | (1 << STATUS_LED2) | (1 << STATUS_LED3));
	wait_ms(500);
}



/**
 * MAIN
 */
int main (void) {
	TWI twi(F_CPU, SCL_CLOCK);
	
	DDRB = 0xff;
	PORTB = 0x00;
	uint8_t status;
	
	twi.init();
	
	wait_ms(100);// pro jistotu kvůli inicializaci LCD
	
	
	LEDInit();
	
	status = LCDInit(twi);
	if (status) blink2(status);
	
    while(1) {
	    LEDInit();
				
		status = LCDSetup(twi);
		if (status) blink2(status);
		
		status = LCDWrite(twi);
		if (status) blink2(status);
		
		wait_ms(5000);
    }
    twi.stop();
}
