/*
* LM75A.c
*
* Created: 26. 10. 2014 17:37:21
*  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
*/

#define F_CPU 1000000UL//1MHz - ATMega8 default

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "LM75A.h"

#define SCL_CLOCK 50000UL//I2C clock 50kHz

#define DEV_ADDR 0b1001000//7bitů!
#define LEDBAR_ADDR 0b0100000
#define ERROR 1
#define SUCCESS 0

/**
 * Inicializace I2C rozhraní. Volá se pouze jednou.
 */
void TWIInit (void) {
	// set SCL to 400kHz
	TWSR = 0x00;
	TWBR = ((F_CPU/SCL_CLOCK)-16)/2;
	// enable TWI
	TWCR = (1 << TWEN);
}

/**
 * Nastaví začátek okna a počká až bude zařízení připraveno
 */
void TWIStart (void) {
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/**
 * Ukončí přenos a počká až bude zařízení připraveno
 */
void TWIStop (void) {
	TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
	while ((TWCR & (1 << TWSTO)));
}

/**
 * Pošle bajt dat po I2C a počká až bude zařízení připraveno
 * @param data data poílaná po I2C
 */
void TWIWrite (uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/**
 * Přečte bajt z I2C a počká až bude zažízení připraveno
 * @return přečtený bajt
 */
uint8_t TWIReadACK (void) {
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/**
 * Přečte bajt z I2C a počká až bude zařízení připraveno. načítání je
 * následováno stopem
 * @return přečtený bajt
 */
uint8_t TWIReadNACK (void) {
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/**
 * Vrátí status I2C
 */
uint8_t TWIGetStatus (void) {
	uint8_t status;
	//mask status
	status = TWSR & 0xf8;
	return status;
}

/**
 * Komunikace s teplotním čidlem
 * @param temp_hi vyšší byte teploty (celé číslo); hodnota  * 1°C
 * @param temp_lo nižší byte teploty (desetinná část); pouze horní 3bity; hodnota * 0,125°C
 */
uint8_t getTemp (uint8_t *temp_hi, uint8_t *temp_lo) {
	
	TWIStart();// S - Start
	if (TWIGetStatus() != TW_START)// je I2C nastartováno?
		return ERROR;
	
	TWIWrite(DEV_ADDR << 1);// pošle SLA+W (slave address + write bit = poslední bit je nula)
	if (TWIGetStatus() != TW_MT_SLA_ACK)
		return ERROR;
	
	TWIWrite(0x00);// Data - nastavení pointeru na 0x00 = čtení teploty
	if (TWIGetStatus() != TW_MT_DATA_ACK)
		return ERROR;
	
	TWIStart();// Rs - Repeated start
	if (TWIGetStatus() != TW_REP_START)
		return ERROR;
	
	TWIWrite((DEV_ADDR << 1) | 1);// pošle SLA+R  (slave address + read bit = poslední bit je nula)
	if (TWIGetStatus() != TW_MR_SLA_ACK)
		return ERROR;
	
	*temp_hi = TWIReadACK();// načte MSB (důležitější bajt) - celá část teploty = poměrně zajímavé číslo
	if (TWIGetStatus() != TW_MR_DATA_ACK)
		return ERROR;
	
	*temp_lo = TWIReadNACK();//načte LSB (méně důležitý bajt) - desetinná část teploty = nezajímá nás
	if (TWIGetStatus() != TW_MR_DATA_NACK)
		return ERROR;
	
	TWIStop();
	
	return SUCCESS;
}


/**
 * Komunikace s LEDBarem
 */
uint8_t sendToLEDBar (uint8_t data) {
	TWIStart();
	if (TWIGetStatus() != TW_START)
		return 1;
	
	TWIWrite(LEDBAR_ADDR << 1);// pošle SLA+W (slave address + write bit)
	if (TWIGetStatus() != TW_MT_SLA_ACK)
		return 2;
	
	TWIWrite(data);// pošle data k zobrazení
	if (TWIGetStatus() != TW_MT_DATA_NACK)
		return 3;
	
	TWIStop();
	
	return 0;
}



/**
 * Nastaví piny pro ledky jako výstup
 */
void LEDInit (void) {
	DDRB = 0x0e;
	PORTB = 0x00;
}



/**
 * Postupně zosvítí a zhasne všechny ledky
 */
void LEDTest (void) {
	PORTB |= 1 << PB1;
	_delay_ms(500);
	PORTB &= ~(1 << PB1);
	
	PORTB |= 1 << PB2;
	_delay_ms(500);
	PORTB &= ~(1 << PB2);
	
	PORTB |= 1 << PB3;
	_delay_ms(500);
	PORTB &= ~(1 << PB3);
}


/**
 * hlavní smyčka (překvapivě)
 */
int main(void) {
	
	uint8_t temp_hi = 0;
	uint8_t temp_lo = 0;
	uint8_t status;
	
	LEDInit();
	LEDTest();
	
	TWIInit();
	
	while(1) {
		PORTB &= ~(1 << PB1);
		
		
		// získání dat z teplotního čidla
		status = getTemp(&temp_hi, &temp_lo);
		if (status) {
			TWIStop();
			
			PORTB |= (1 << PB1);
			PORTB |= (1 << PB3);
			
			for (uint8_t i = 0; i < status; i++) {
				PORTB |= (1 << PB2);
				_delay_ms(250);
				
				PORTB &= ~(1 << PB2);
				_delay_ms(250);
			}
			continue;
		}
		
		
		
		// poslání dat na LEDBar
		status = sendToLEDBar(temp_hi);
		if (status) {
			TWIStop();
			
			PORTB |= (1 << PB1);
			PORTB |= (1 << PB3);
			
			for (uint8_t i = 0; i < status; i++) {
				PORTB |= (1 << PB2);
				_delay_ms(250);
				
				PORTB &= ~(1 << PB2);
				_delay_ms(250);
			}
			
			_delay_ms(1000);
		}
		
		
		
		
		// celá část teploty (<počet bliknutí> * 1°C)
		PORTB &= ~(1 << PB3);
		
		if (temp_hi >= 20) {
			temp_hi -= 20;
			PORTB |= (1 << PB2);
		} else {
			PORTB &= ~(1 << PB2);
		}
		
		for (uint8_t i = 0; i < temp_hi; i++) {
			PORTB |= (1 << PB3);
			_delay_ms(250);
			
			PORTB &= ~(1 << PB3);
			_delay_ms(250);
		}
		
		_delay_ms(1000);
		
		
		
		// desetinná část teploty (<počet bliknutí> * 0,125°C)
		PORTB &= ~(1 << PB2);
		
		for (uint8_t i = 0; i < temp_lo >> 5; i++) {
			PORTB |= (1 << PB3);
			_delay_ms(250);
			
			PORTB &= ~(1 << PB3);
			_delay_ms(250);
		}
	
		_delay_ms(1000);
    }
}
