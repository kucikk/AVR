/*
 * twi_slave.cpp
 *
 * Created: 2. 1. 2015 15:30:04
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */ 

#define F_CPU 1000000L

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "twi-slave.h"

#define LEDBAR_ADDR 0b0100000

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
 * Nastaví patřičné piny jako výstupní
 */
void LEDBarInit (void) {
	LED0_DDR |= (1 << LED0);
	LED1_DDR |= (1 << LED1);
	LED2_DDR |= (1 << LED2);
	LED3_DDR |= (1 << LED3);
	LED4_DDR |= (1 << LED4);
	LED5_DDR |= (1 << LED5);
	LED6_DDR |= (1 << LED6);
	LED7_DDR |= (1 << LED7);
	LED8_DDR |= (1 << LED8);
	LED9_DDR |= (1 << LED9);
}

/**
 * Efektně otestuje funkčnost ledek
 */
void LEDBarTest() {
	LED0_PORT |= 1 << LED0;
	LED1_PORT |= 1 << LED1;
	LED2_PORT |= 1 << LED2;
	LED3_PORT |= 1 << LED3;
	LED4_PORT |= 1 << LED4;
	LED5_PORT |= 1 << LED5;
	LED6_PORT |= 1 << LED6;
	LED7_PORT |= 1 << LED7;
	LED8_PORT |= 1 << LED8;
	LED9_PORT |= 1 << LED9;
	wait_ms(100);
	
	LED0_PORT &= ~(1 << LED0);
	LED1_PORT &= ~(1 << LED1);
	LED2_PORT &= ~(1 << LED2);
	LED3_PORT &= ~(1 << LED3);
	LED4_PORT &= ~(1 << LED4);
	LED5_PORT &= ~(1 << LED5);
	LED6_PORT &= ~(1 << LED6);
	LED7_PORT &= ~(1 << LED7);
	LED8_PORT &= ~(1 << LED8);
	LED9_PORT &= ~(1 << LED9);
	
	// knightrider
	LED0_PORT |= 1 << LED0;
	wait_ms(100);
	
	LED0_PORT &= ~(1 << LED0);
	LED1_PORT |= 1 << LED1;
	wait_ms(100);
	
	LED1_PORT &= ~(1 << LED1);
	LED2_PORT |= 1 << LED2;
	wait_ms(100);
	
	LED2_PORT &= ~(1 << LED2);
	LED3_PORT |= 1 << LED3;
	wait_ms(100);
	
	LED3_PORT &= ~(1 << LED3);
	LED4_PORT |= 1 << LED4;
	wait_ms(100);
	
	LED4_PORT &= ~(1 << LED4);
	LED5_PORT |= 1 << LED5;
	wait_ms(100);
	
	LED5_PORT &= ~(1 << LED5);
	LED6_PORT |= 1 << LED6;
	wait_ms(100);
	
	LED6_PORT &= ~(1 << LED6);
	LED7_PORT |= 1 << LED7;
	wait_ms(100);
	
	LED7_PORT &= ~(1 << LED7);
	LED8_PORT |= 1 << LED8;
	wait_ms(100);
	
	LED8_PORT &= ~(1 << LED8);
	LED9_PORT |= 1 << LED9;
	wait_ms(100);
	
	LED9_PORT &= ~(1 << LED9);
	LED8_PORT |= 1 << LED8;
	wait_ms(100);
	
	LED8_PORT &= ~(1 << LED8);
	LED7_PORT |= 1 << LED7;
	wait_ms(100);
	
	LED7_PORT &= ~(1 << LED7);
	LED6_PORT |= 1 << LED6;
	wait_ms(100);
	
	LED6_PORT &= ~(1 << LED6);
	LED5_PORT |= 1 << LED5;
	wait_ms(100);
	
	LED5_PORT &= ~(1 << LED5);
	LED4_PORT |= 1 << LED4;
	wait_ms(100);
	
	LED4_PORT &= ~(1 << LED4);
	LED3_PORT |= 1 << LED3;
	wait_ms(100);
	
	LED3_PORT &= ~(1 << LED3);
	LED2_PORT |= 1 << LED2;
	wait_ms(100);
	
	LED2_PORT &= ~(1 << LED2);
	LED1_PORT |= 1 << LED1;
	wait_ms(100);
	
	LED1_PORT &= ~(1 << LED1);
	LED0_PORT |= 1 << LED0;
	wait_ms(100);
	
	LED0_PORT &= ~(1 << LED0);
}


void LEDBarSetValue (uint16_t value) {
	
	if (value & 1 << 0) LED0_PORT |=  (1 << LED0);
	else LED0_PORT &= ~(1 << LED0);
	
	if (value & 1 << 1) LED1_PORT |=  (1 << LED1);
	else LED1_PORT &= ~(1 << LED1);
	
	if (value & 1 << 2) LED2_PORT |=  (1 << LED2);
	else LED2_PORT &= ~(1 << LED2);
	
	if (value & 1 << 3) LED3_PORT |=  (1 << LED3);
	else LED3_PORT &= ~(1 << LED3);
	
	if (value & 1 << 4) LED4_PORT |=  (1 << LED4);
	else LED4_PORT &= ~(1 << LED4);
	
	if (value & 1 << 5) LED5_PORT |=  (1 << LED5);
	else LED5_PORT &= ~(1 << LED5);
	
	if (value & 1 << 6) LED6_PORT |=  (1 << LED6);
	else LED6_PORT &= ~(1 << LED6);
	
	if (value & 1 << 7) LED7_PORT |=  (1 << LED7);
	else LED7_PORT &= ~(1 << LED7);
	
	if (value & 1 << 8) LED8_PORT |=  (1 << LED8);
	else LED8_PORT &= ~(1 << LED8);
	
	if (value & 1 << 9) LED9_PORT |=  (1 << LED9);
	else LED9_PORT &= ~(1 << LED9);
}



#define USI_PORT PORTB
#define USI_PIN  PINB
#define USI_DDR  DDRB
#define USI_SCL  PB7
#define USI_SDA  PB5

volatile uint8_t twi_state = 0x00;
volatile uint8_t data = 0x00;


int main (void) {
	
	LEDBarInit();
	LEDBarTest();
	
	USI_PORT |= (1 << USI_SCL);//SCL high
	USI_PORT |= (1 << USI_SDA);//SDA high
	USI_DDR |= (1 << USI_SCL);//SCL as output
	USI_DDR &= ~(1 << USI_SDA);//SDA as input
	
	USICR = (1 << USISIE) |//Enable: Start Condition Interrupt
	        (0 << USIOIE) |//Disable: Overflow Interupt
	        (1 << USIWM1) | (0 << USIWM0) |//Mode: Two-wite; No USI counter overflow hold
			(1 << USICS1) | (0 << USICS0) | (0 << USICLK) |//Clock source: External, positive edge
			(0 << USITC);//0
			
	USISR = 0xf0;//clear flags, reset counter
	
	sei();
	
	while (1) {
		/*
		LEDBarSetValue(0b1100000011);
		wait_ms(500);
		*/
		LEDBarSetValue(data);
		wait_ms(500);
		/*
		LEDBarSetValue(0b0011111100);
		wait_ms(500);
		*/
		
		
		
		/*
        LED0_PORT |= 1 << LED0;
        wait_ms(100);
        LED1_PORT |= 1 << LED1;
        wait_ms(100);
        LED2_PORT |= 1 << LED2;
        wait_ms(100);
        LED3_PORT |= 1 << LED3;
        wait_ms(100);
        LED4_PORT |= 1 << LED4;
        wait_ms(100);
        LED5_PORT |= 1 << LED5;
        wait_ms(100);
        LED6_PORT |= 1 << LED6;
        wait_ms(100);
        LED7_PORT |= 1 << LED7;
        wait_ms(100);
        LED8_PORT |= 1 << LED8;
        wait_ms(100);
        LED9_PORT |= 1 << LED9;
        wait_ms(100);
		
		LED9_PORT &= ~(1 << LED9);
		wait_ms(100);
		LED8_PORT &= ~(1 << LED8);
		wait_ms(100);
		LED7_PORT &= ~(1 << LED7);
		wait_ms(100);
		LED6_PORT &= ~(1 << LED6);
		wait_ms(100);
		LED5_PORT &= ~(1 << LED5);
		wait_ms(100);
		LED4_PORT &= ~(1 << LED4);
		wait_ms(100);
		LED3_PORT &= ~(1 << LED3);
		wait_ms(100);
		LED2_PORT &= ~(1 << LED2);
		wait_ms(100);
		LED1_PORT &= ~(1 << LED1);
		wait_ms(100);
		LED0_PORT &= ~(1 << LED0);
		wait_ms(100);
		*/
		
		/*
		LEDBarSetValue(0b0101010101);
		wait_ms(1000);
		
		LEDBarSetValue(0b1010101010);
		wait_ms(1000);
		*/
    }
}


/**
 * Začátek přenosu
 */
ISR(USI_START_vect) {
	twi_state = 0x00;
	USI_DDR &= ~(1 << USI_SDA);//SDA as input
	
	while (
		(USI_PIN & (1 << USI_SCL)) &//čekáme až bude SCL low
		(USISR & (1 << USIPF))//pokud se objeví Stop flag, tak končíme, aby jsme tu nezkejsli navždy
	);
	
	USICR = (1 << USISIE) |//Enable: Start condition interupt - ponecháme kvůli restartu
	        (1 << USIOIE) |//Enable: Overflow Interrupt
			(1 << USIWM1) | (1 << USIWM0) |//Mode: Two-wire 2 (USI counter overflow hold)
			(1 << USICS1) | (0 << USICS0) | (0 << USICLK) |//Clock source: External, positive edge
			(0 << USITC);
	
	USISR = 0xf0;//clear flags; reset counter;
}

/**
 * Zpracování průběhu přenosu
 */
ISR(USI_OVERFLOW_vect) {
	switch (twi_state) {
		case 0x00://check address + send ACK; jinak reset USI
			if (USIDR == 0 || (USIDR >> 1 == LEDBAR_ADDR)) {
				// OK
				if (USIDR & 0x01) {
					twi_state = 0x00;
					//TODO tohle zatím nenastává - reset USI
					USICR = (1 << USISIE) |//Enable: Start Condition Interrupt
					        (0 << USIOIE) |//Disable: Overflow Interrupt
					        (1 << USIWM1) | (0 << USIWM0) |//Mode: Two-wire; No USI counter overflow hold
					        (1 << USICS1) | (0 << USICS0) | (0 << USICLK) |//Clock source: External, positive edge
					        (0 << USITC);
					
					USISR = (0 << USISIF) |//Set: Start Condition Flag
					        (1 << USIOIF) |//Clear: Oveflow flag
					        (1 << USIPF) |//Clear: Stop flag
					        (1 << USIDC) |//Clear: Data Output Collision
					        (0x0 << USICNT0);//set USI sounter to shift 8 bits
				} else {
					// W - poslední bit je 0 - master chce zapisovat
					twi_state = 0x01;//Goto: request data
					
					
					USIDR = 0;//prepare ACK
					USI_DDR |= (1 << USI_SDA);//SDA as output
					USISR = (0 << USISIF) |//Set: Start Condition Flag
							(1 << USIOIF) |//Clear: Oveflow flag
							(1 << USIPF) |//Clear: Stop flag
							(1 << USIDC) |//Clear: Data Output Collision
							(0x0e << USICNT0);//set USI sounter to shift 1 bit
				}
			} else {
				USICR = (1 << USISIE) |//Enable: Start Condition Interrupt
				        (0 << USIOIE) |//Disable: Overflow Interrupt
						(1 << USIWM1) | (0 << USIWM0) |//Mode: Two-wire; No USI counter overflow hold
						(1 << USICS1) | (0 << USICS0) | (0 << USICLK) |//Clock source: External, positive edge
						(0 << USITC);
				
				USISR = (0 << USISIF) |//Set: Start Condition Flag
				        (1 << USIOIF) |//Clear: Oveflow flag
				        (1 << USIPF) |//Clear: Stop flag
				        (1 << USIDC) |//Clear: Data Output Collision
				        (0x0 << USICNT0);//set USI sounter to shift 8 bits
			}
			break;
			
		//// Master read data mode
		case 0x01://request data
			twi_state = 0x02;
			USI_DDR &= ~(1 << USI_SDA);//SDA as input
			USISR = (0 << USISIF) |//Set: Start Condition Flag
			        (1 << USIOIF) |//Clear: Oveflow flag
			        (1 << USIPF) |//Clear: Stop flag
			        (1 << USIDC) |//Clear: Data Output Collision
			        (0x0 << USICNT0);//set USI sounter to shift 8 bits
			break;
		
		case 0x02://get data from USIDR + ACK
			data = USIDR;
			
			twi_state = 0x01;
			
			USIDR = 0;
			USI_DDR |= (1 << USI_SDA);
			USISR = (0 << USISIF) |
			        (1 << USIOIF) |//Clear: Oveflow flag
			        (1 << USIPF) |//Clear: Stop flag
			        (1 << USIDC) |//Clear: Data Output Collision
					(0x0e << USICNT0);//set USI to shift 1 bit
			        
	}
}
