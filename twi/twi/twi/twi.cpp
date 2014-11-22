/*
 * twi.c
 *
 * Created: 22. 11. 2014 10:09:26
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */
#include <avr/io.h>
#include <util/twi.h>
#include "twi.h"

/************************************************************************/
/* Konstruktor                                                          */
/************************************************************************/
TWI::TWI (uint32_t cpu, uint32_t clock) {
	this->cpu = cpu;
	this->clock = clock;
}

/************************************************************************/
/* Inicializace I2C rozhraní. Volá se pouze jednou.                     */
/************************************************************************/
void TWI::init () {
	// set SCL to 400kHz
	TWSR = 0x00;
	TWBR = ((this->cpu/this->clock)-16)/2;
	// enable TWI
	TWCR = (1 << TWEN);
}

/************************************************************************/
/* Nastaví začátek okna a počká až bude zařízení připraveno             */
/************************************************************************/
void TWI::start (void) {
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/************************************************************************/
/* Ukončí přenos a počká až bude zařízení připraveno                    */
/************************************************************************/
void TWI::stop (void) {
	TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
	while ((TWCR & (1 << TWSTO)));
}

/************************************************************************/
/* Pošle bajt dat po I2C a počká až bude zařízení připraveno            */
/************************************************************************/
void TWI::write (uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/************************************************************************/
/* Přečte bajt z I2C a počká až bude zažízení připraveno                */
/* Return: přečtený bajt                                                */
/************************************************************************/
uint8_t TWI::readACK (void) {
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/************************************************************************/
/* Přečte bajt z I2C a počká až bude zařízení připraveno. načítání je   */
/* následováno stopem                                                   */
/* Return: přečtený bajt                                                */
/************************************************************************/
uint8_t TWI::readNACK (void) {
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/************************************************************************/
/* Vrátí status I2C                                                     */
/************************************************************************/
uint8_t TWI::getStatus (void) {
	uint8_t status;
	//mask status
	status = TWSR & 0xf8;
	return status;
}

/************************************************************************/
/* Odpovídá status?                                                     */
/************************************************************************/
bool TWI::statusIs (uint16_t expected) {
	return this->getStatus() == expected;
}
