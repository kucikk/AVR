/*
 * usart.cpp
 *
 * Created: 6. 12. 2014 13:04:14
 *  Author: Tomáš
 */ 

#define F_CPU 1000000L //1MHz

#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 2400
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

ISR (USART_RXC_vect) {
	PORTC |= (1 << PC4);
}

void USART_Init (void) {
	UBRRL = BAUD_PRESCALE;
	UBRRH = (BAUD_PRESCALE >> 8);
	UCSRB = ((1 << TXEN) | (1 << RXEN) | (1 << RXCIE));
}

void USART_SendByte (uint8_t u8Data) {
	while ((UCSRA & (1 << UDRE)) == 0);
	UDR = u8Data;
}

uint8_t USART_RecieveByte () {
	while ((UCSRA & (1 << RXC)) == 0);
	return UDR;
}

void USART_SendString (char const *string) {
	for (uint8_t i = 0; i < strlen(string); i++) {
		USART_SendByte(string[i]);
	}
}

void Led_init (void) {
	DDRB = 0xff;
	PORTB = 0xff;
}

int main(void) {
	USART_Init();
	Led_init();
	DDRC = 0xff;
	PORTC = 0xff;
	bool sth = 0;
	_delay_ms(100);
	sei();
    while (1) {
		if (sth) {
			PORTC &= ~(1 << PC5);
			sth = 0;
		} else {
			PORTC |= (1 << PC5);
			sth = 1;
		}
		
		USART_SendString("Lorem ipsum dolor sit amet, consectetuer adipiscing elit.\n");
		
		
		PORTC &= ~(1 << PC4);
    }
}