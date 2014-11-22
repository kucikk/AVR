/*
* LM75A.c
*
* Created: 26. 10. 2014 17:37:21
*  Author: Tomáš Kučera
*/

#define F_CPU 1000000UL//1MHz - ATMega8 default

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "LM75A.h"

#define SCL_CLOCK 50000UL//I2C clock 50kHz

#define DEV_ADDR 0b01001000//7bitů!
#define ERROR 1
#define SUCCESS 0

/************************************************************************/
/* Inicializace I2C rozhraní. Volá se pouze jednou.                     */
/************************************************************************/
void TWIInit (void) {
	// set SCL to 400kHz
	TWSR = 0x00;
	TWBR = ((F_CPU/SCL_CLOCK)-16)/2;
	// enable TWI
	TWCR = (1 << TWEN);
}

/************************************************************************/
/* Nastaví začátek okna a počká až bude zařízení připraveno             */
/************************************************************************/
void TWIStart (void) {
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/************************************************************************/
/* Ukončí přenos a počká až bude zařízení připraveno                    */
/************************************************************************/
void TWIStop (void) {
	TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
	while ((TWCR & (1 << TWSTO)));
}

/************************************************************************/
/* Pošle bajt dat po I2C a počká až bude zařízení připraveno            */
/************************************************************************/
void TWIWrite (uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

/************************************************************************/
/* Přečte bajt z I2C a počká až bude zažízení připraveno                */
/* Return: přečtený bajt                                                */
/************************************************************************/
uint8_t TWIReadACK (void) {
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/************************************************************************/
/* Přečte bajt z I2C a počká až bude zařízení připraveno. načítání je   */
/* následováno stopem                                                   */
/* Return: přečtený bajt                                                */
/************************************************************************/
uint8_t TWIReadNACK (void) {
	TWCR = (1 << TWINT)|(1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

/************************************************************************/
/* Vrátí status I2C                                                     */
/************************************************************************/
uint8_t TWIGetStatus (void) {
	uint8_t status;
	//mask status
	status = TWSR & 0xf8;
	return status;
}

/************************************************************************/
/* Pokusí se přečíst teplotu z čidla                                    */
/************************************************************************/
uint8_t getTemp (uint8_t *u8data) {
	
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
	
	*u8data = TWIReadACK();// načte MSB (důležitější bajt) - celá část teploty = poměrně zajímavé číslo
	if (TWIGetStatus() != TW_MR_DATA_ACK)
		return ERROR;
	
	TWIReadNACK();//načte LSB (méně důležitý bajt) - desetinná část teploty = nezajímá nás
	if (TWIGetStatus() != TW_MR_DATA_NACK)
		return ERROR;
	
	TWIStop();
	
	return SUCCESS;
}

int main(void) {
	
	uint8_t temp = 0;
	//uint8_t t = 0;
	
	DDRB = 0xff;
	PORTB = 0xff;
	
	DDRD = 0x04;
	PORTD = 0x00;
	
	_delay_ms(1000);
	PORTB = 0x00;
	_delay_ms(1000);
	
	TWIInit();
	
	while(1) {
		if (getTemp(&temp) == ERROR)
			continue;
		PORTB = temp;
		/*
		if ((temp & 0x80) == 1) {// záporné číslo
			PORTB = 0x10;// poslední ledka
		} else {
			t = temp & 0x7f;
			
			if (t > 30) {
				PORTB = 0x10;//poslední ledka
			} else if (t > 29) {
				PORTB = 0x1f;//všechny ledky
			} else if (t > 28) {
				PORTB = 0x0f;//krom poslední ledky
			} else if (t > 27) {
				PORTB = 0x07;//první 3 ledky
			} else if (t > 26) {
				PORTB = 0x03;//první 2 ledky
			} else if (t > 25) {
				PORTB = 0x01;//první ledka
			} else if (t > 24) {
				PORTB = 0x02;//předposlední ledka
			} else if (t > 23) {
				PORTB = 0x04;//prostřední ledka
			} else {
				PORTB = 0x08;//předposlední ledka
			}
		}
		*/
    }
}
