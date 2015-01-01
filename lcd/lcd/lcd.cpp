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
	blink(count, 150);
}

/**
 * BLINK - 3 times, 100ms delay
 */
void blink () {
	blink(3, 100);
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
	blink2(count, 100);
}

/**
 * BLINK2 - 3 times, 100ms delay
 */
void blink2 () {
	blink2(3, 100);
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
	blink3(count, 500);
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



/**
 * LCD TEST
 */
bool test (TWI twi) {
	
	toggleLed3();
	twi.start();// S - Start
	if (!twi.statusIs(TW_START)) {// je I2C nastartováno?
		return 1;
	}
	//blink2(1);
	
	toggleLed3();
	twi.write(DEV_ADDR << 1);// pošle SLA+W (slave address + write bit = poslední bit je nula)
	if (!twi.statusIs(TW_MT_SLA_ACK)) {
		return 2;
	}
	//blink2(2);

	//init
	toggleLed3();
	twi.write(0b00100011);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 3;
	}
	//blink2(3);
	//wait_ms(3);
	
	toggleLed3();
	twi.write(0b00000011);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 4;
	}
	//blink2(4);
	wait_ms(2);
	
	toggleLed3();
	twi.write(0b00100011);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 5;
	}
	//blink2(5);
	wait_ms(1);
	
	toggleLed3();
	twi.write(0b00000011);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 6;
	}
	//blink2(6);
	
	toggleLed3();
	twi.write(0b00000011);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 7;
	}
	//blink2(7);
	
	// 4 bitová instrukce najednou - nastavuje komunikaci na 4-bitovou
	toggleLed3();
	twi.write(0b00000010);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 8;
	}
	//blink2(8);


	// nastavení displeje
	toggleLed3();
	twi.write(0b00000010);//(RS, R/W, E, ?, D4, D5, D6=1, D7=4bit data length)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 9;
	}
	
	twi.write(0b00001000);//(RS, R/W, E, ?, D4=2 řádky, D5=5x8 font, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 10;
	}
	//blink(1);


	// display on/off, cursor
	toggleLed3();
	twi.write(0b00000000);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 11;
	}
	
	twi.write(0b00001111);//(RS, R/W, E, ?, D4=1, D5=display on, D6=cursor on, D7=blinking cursor)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 12;
	}
	//blink(2);


	// display clear
	toggleLed3();
	twi.write(0b00000000);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 13;
	}
	
	twi.write(0b00000001);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 14;
	}
	//blink(3);
	
	
	// entry mode set - cursor move direction, display shift
	toggleLed3();
	twi.write(0b00000000);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 15;
	}

	twi.write(0b00000110);//(RS, R/W, E, ?, D4, D5=1, D6=inc/decrement, D7=accompanies display shift)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 16;
	}
	//blink(4);


	// write H
	toggleLed3();
	twi.write(0b10000100);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 15;
	}
	
	twi.write(0b10001000);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 16;
	}
	//blink(4);


	// write I
	toggleLed3();
	twi.write(0b10000100);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 15;
	}
	
	twi.write(0b10001001);//(RS, R/W, E, ?, D4, D5, D6, D7)
	if (!twi.statusIs(TW_MT_DATA_ACK)) {
		return 16;
	}
	blink(5);
		
	return SUCCESS;
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
	
	twi.init();
	
	wait_ms(100);//pro jistotu
	
    while(1) {
		LEDInit();
		if (test(twi) == ERROR) {
			wait_ms(500);
			blink();
		}
		twi.stop();
		wait_ms(5000);
    }
}
