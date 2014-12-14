/*
 * uart.cpp
 *
 * Created: 14. 12. 2014 14:18:25
 *  Author: Tomáš Kučera <me@kucikk.cz> http://www.kucikk.cz
 */
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

volatile bool uart::initialized = false;
volatile uint8_t uart::last_rx_char;
volatile uint8_t uart::last_rx_err;

volatile uint8_t uart::rx_buf[UART_RX_BUF_SIZE];
volatile uint8_t uart::rx_head = 0;
volatile uint8_t uart::rx_tail = 0;

volatile uint8_t uart::tx_buf[UART_TX_BUF_SIZE];
volatile uint8_t uart::tx_head = 0;
volatile uint8_t uart::tx_tail = 0;


/************************************************************************/
/* Přerušení pro čtení bajtu                                            */
/************************************************************************/
ISR(USART_RXC_vect) {
	if (uart::initialized != true)
		return;
		
	uint8_t tmphead;
	
	tmphead = (uart::rx_head + 1) & UART_RX_BUF_MASK;
	
	uart::last_rx_err = 0x00;
	
	uart::last_rx_err |= (UCSRA & (1 << FE)) ? UART_FRAME_ERROR : 0;
	uart::last_rx_err |= (UCSRA & (1 << DOR)) ? UART_OVERRUN_ERROR : 0;
	
	if (uart::rx_tail == tmphead) {
		uart::last_rx_err |= UART_BUFFER_OVERFLOW;
	} else {
		uart::rx_buf[tmphead] = UDR;
		uart::rx_head = tmphead;
	}
}

/************************************************************************/
/* Přerušení pro zápis bajtu                                            */
/************************************************************************/
ISR(USART_UDRE_vect) {
	if (uart::initialized != true)
		return;
		
	if (uart::tx_head != uart::tx_tail) {
		//vypočteme nový konec zásobníku - maska se používá kvůli kruhovému zásobníku
		uart::tx_tail = (uart::tx_tail + 1) & UART_TX_BUF_MASK;
		UDR = uart::tx_buf[uart::tx_tail];//pošleme poslední bajt
	} else {
		// žádná data k odeslání, vypneme přerušení
		UCSRB &= ~(1 << UDRIE);
	}	
}

/************************************************************************/
/* Inicializace uart rozhraní. Volá se jen jednou                       */
/************************************************************************/
void uart::init(uint32_t cpu, uint32_t clock) {
	if (uart::initialized == true)
		return;
	
	// výpočet baudrate
	uint16_t baud_prescale = ((cpu / (clock * 16UL))) - 1;
	// nastavení baudrate
	UBRRL = (uint8_t) baud_prescale;
	UBRRH = (uint8_t) (baud_prescale >> 8);
	
	//enable transmitter, reciever and recieve complete interrupt
	UCSRB = ((1 << TXEN) | (1 << RXEN) | (1 << RXCIE));
	
	// frame format
	//UCSRC = UCSRC;
	uart::initialized = true;
}

/************************************************************************/
/* Přečte znak                                                          */
/************************************************************************/
uint16_t uart::getc () {
	if (uart::rx_head == uart::tx_tail) {
		return (UART_NO_DATA << 8) | 0x00;//zásobník je prázdný
	}
	
	uart::rx_tail = (uart::rx_tail + 1) & UART_RX_BUF_MASK;
	
	return (uart::last_rx_err << 8) | uart::rx_buf[uart::rx_tail];
}

/************************************************************************/
/* Zapíše znak                                                          */
/************************************************************************/
void uart::putc (uint8_t chr) {
	uint8_t tmphead;
	
	tmphead = (uart::tx_head + 1) & UART_TX_BUF_MASK;
	
	// TOOD může dojít k nekonečné smyčce pokud nebude nejprve inicializováno
	while (tmphead == uart::tx_tail) {
		;// počkáme až se v bufferu uvolní místo
	}
	
	uart::tx_buf[tmphead] = chr;// uložíme bajt do bufferu
	uart::tx_head = tmphead;// posuneme začátek bufferu o bajt dál
	
	// zapneme přerušení pro odesílání dat
	UCSRB |= (1 << UDRIE);
}

/************************************************************************/
/* Zapíše string                                                        */
/************************************************************************/
void uart::puts (const char *str) {
	while (*str) {
		uart::putc(*str++);
	}
}
