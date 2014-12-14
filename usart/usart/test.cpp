/*
 * main.cpp
 *
 * Created: 14. 12. 2014 13:04:14
 *  Author: Tomáš Kučera <me@kucikk.cz> http://www.kucikk.cz
 */ 

#define F_CPU 1000000L //1MHz
#define USART_BAUDRATE 2400 //UART clock

#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart/uart.h"

void Led_init (void) {
	DDRC = 0xff;
	
	PORTC = 0xff;
	_delay_ms(50);
	PORTC = 0x00;
	_delay_ms(50);
	PORTC = 0xff;
	_delay_ms(50);
	PORTC = 0x00;
}

int main (void) {
	uart::init(F_CPU, USART_BAUDRATE);
	
	Led_init();
	
	sei();
	
	uint8_t str = 'T';
	uint16_t c;
	uint8_t chr;
	uint8_t state;
	uint8_t new_portc;
	
	while (1) {
		new_portc = 0x00;
		
		uart::puts("Start:\r\n======\r\n");
		uart::putc(str);
		uart::puts("\n");
		
		c = uart::getc();
		chr = c & 0xff;
		state = c >> 8;
		
		if (state > 0) {
			uart::puts("State: KO\r\n----------\r\n");
			const char *ret = "Num: 0b________\r\n";
			char *pch = strchr(ret, 'b');
			pch++;
			
			for (uint8_t i = 0; i < 8; i++) {
				pch[i] = (state & (1 << (7 - i))) ? '1' : '0';
			}
			
			uart::puts(ret);
			uart::puts("----------\r\n");
			
			new_portc |= ((1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5));
		} else {
			uart::puts("State: OK\r\n");
			
			if (chr > 0) {
				new_portc |= (1 << PC4);
				// znak
				uart::puts("Byte: yes\r\n");
				
				if (chr < 0x20) {
					// řídící znak
					uart::puts("Type: control character\r\n");
					
					new_portc |= (1 << PC3);
				} else {
					uart::puts("Type: character\r\n");
					
					if (chr > 0x7f) {
						// speciální znak (písmeno) - nad 128
						uart::puts("Special: yes\r\n");
						new_portc |= (1 << PC2);
					}
					
					str = chr;
				}
			} else {
				new_portc |= (1 << PC5);
			}
			
		}
		
		uart::puts("----------\r\n");
		
		PORTC = new_portc;
		
		_delay_ms(50);
		PORTC &= ~((1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5));
		_delay_ms(100);
	}
}