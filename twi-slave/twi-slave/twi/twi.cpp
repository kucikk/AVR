/*
 * twi.c
 *
 * Created: 2. 1. 2015 19:09:26
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */

#include <avr/io.h>
#include <util/twi.h>
#include "twi.h"


/**
 * Inicializace I2C rozhraní. Volá se pouze jednou.
 * @param cpu frekvence mcu
 * @param clock frekvence rozhranní
 */
void twi::init (uint32_t cpu, uint32_t clock) {
	// set SCL frequency
	USISR = 0x00;
	TWBR = ((cpu/clock)-16)/2;
	// enable TWI
	USICR = (1 << USISIE) | (1 << USIWM1) | (0 << USIWM0);
	TWCR = (1 << TWEN);
}

/**
 * Nastaví začátek okna a počká až bude zařízení připraveno
 */
void twi::start (void) {
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/**
 * Ukončí přenos a počká až bude zařízení připraveno
 */
void twi::stop (void) {
	TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
	while ((TWCR & (1 << TWSTO)));
}

/**
 * Pošle bajt dat po I2C a počká až bude zařízení připraveno
 * @param data byjt který chceme poslat po TWI
 */
void twi::write (uint8_t data) {
	USIDR = data;
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/**
 * Přečte bajt z I2C a počká až bude zažízení připraveno
 * @return přečtený bajt
 */
uint8_t twi::readAck (void) {
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/**
 * Přečte bajt z I2C a počká až bude zařízení připraveno. načítání je
 * následováno stopem
 * @return přečtený bajt
 */
uint8_t twi::readNack (void) {
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/**
 * Vrátí status I2C
 */
uint8_t twi::getStatus (void) {
	uint8_t status;
	//mask status
	status = TWSR & 0xf8;
	return status;
}

/**
 * Srovná aktuální a očekávaný status
 * @param expected očekávaný status
 * @return odpovídá status očekávání?
 */
bool twi::statusIs (uint16_t expected) {
	return twi::getStatus() == expected;
}
