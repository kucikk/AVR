/*
 * uart.h
 *
 * Created: 14. 12. 2014 14:18:37
 *  Author: Tomáš Kučera <me@kucikk.cz> http://www.kucikk.cz
 */


#ifndef UART_H_
#define UART_H_

#define UART_FRAME_ERROR     0x08 // framing error - UART
#define UART_OVERRUN_ERROR   0x04 // overrun error - UART
#define UART_BUFFER_OVERFLOW 0x02 // ringbuffer overflow
#define UART_NO_DATA         0x01 // no data recieved

#define UART_RX_BUF_SIZE 8
#define UART_TX_BUF_SIZE 8

#define UART_RX_BUF_MASK (UART_RX_BUF_SIZE - 1)
#define UART_TX_BUF_MASK (UART_TX_BUF_SIZE - 1)

namespace uart {
	extern volatile bool initialized;
	extern volatile uint8_t last_rx_char;
	extern volatile uint8_t last_rx_err;
	
	extern volatile uint8_t rx_buf[UART_RX_BUF_SIZE];
	extern volatile uint8_t rx_head;
	extern volatile uint8_t rx_tail;
	
	extern volatile uint8_t tx_buf[UART_TX_BUF_SIZE];
	extern volatile uint8_t tx_head;
	extern volatile uint8_t tx_tail;

	void init(uint32_t, uint32_t);
	uint16_t getc(void);
	void putc(uint8_t);
	void puts(const char*);
};

#endif /* UART_H_ */