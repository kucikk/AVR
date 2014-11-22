/*
 * LM75A.c
 *
 * Created: 26. 10. 2014 17:37:21
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */

#define F_CPU 1000000UL//1MHz - ATMega8 default
#define SCL_CLOCK 50000UL//I2C clock 50kHz

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "twi/twi.h"

#define DEV_ADDR 0b01001000//7bitů!
#define ERROR 1
#define SUCCESS 0

/************************************************************************/
/* Pokusí se přečíst teplotu z čidla                                    */
/************************************************************************/
uint8_t getTemp (TWI twi, uint8_t *temp_hi, uint8_t *temp_lo) {

	twi.start();// S - Start
	if (!twi.statusIs(TW_START))// je I2C nastartováno?
		return ERROR;
		
	twi.write(DEV_ADDR << 1);// pošle SLA+W (slave address + write bit = poslední bit je nula)
	if (!twi.statusIs(TW_MT_SLA_ACK))
		return ERROR;
			
	twi.write(0x00);// Data - nastavení pointeru na 0x00 = čtení teploty
	if (!twi.statusIs(TW_MT_DATA_ACK))
		return ERROR;
			
	twi.start();// Rs - Repeated start
	if (!twi.statusIs(TW_REP_START))
		return ERROR;
			
	twi.write((DEV_ADDR << 1) | 1);// pošle SLA+R  (slave address + read bit = poslední bit je nula)
	if (!twi.statusIs(TW_MR_SLA_ACK))
		return ERROR;
			
	*temp_hi = twi.readACK();// načte MSB (důležitější bajt) - celá část teploty = poměrně zajímavé číslo
	if (!twi.statusIs(TW_MR_DATA_ACK))
		return ERROR;
			
	*temp_lo = twi.readNACK();//načte LSB (méně důležitý bajt) - desetinná část teploty
	if (!twi.statusIs(TW_MR_DATA_NACK))
		return ERROR;
			
	twi.stop();
	
	return SUCCESS;
}

int main(void) {
	TWI twi(F_CPU, SCL_CLOCK);
	
	uint8_t temp_hi = 0;
	uint8_t temp_lo = 0;
	
	DDRB = 0xff;
	PORTB = 0xff;
	
	DDRD = 0x0f;
	PORTD = 0x07;
	_delay_ms(500);
	PORTB = 0x00;
	PORTD = 0x00;
	_delay_ms(500);
	PORTB = 0xff;
	PORTD = 0x07;
	_delay_ms(500);
	PORTB = 0x00;
	PORTD = 0x00;
	_delay_ms(500);
	PORTB = 0xff;
	PORTD = 0x07;
	_delay_ms(500);
	PORTB = 0x00;
	PORTD = 0x00;
	_delay_ms(500);
	
	twi.init();
	
	while(1) {
		if (getTemp(twi, &temp_hi, &temp_lo) == ERROR)
			continue;
		PORTB = temp_hi;
		PORTD = temp_lo & 0x07;
    }
}
