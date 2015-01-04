/*
 * twi.h
 *
 * Created: 2. 1. 2015 19:09:35
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */ 


#ifndef TWI_H_
#define TWI_H_

namespace twi {
	void init (uint32_t, uint32_t);
	void start (void);
	void stop (void);
	void write (void);
	uint8_t readAck (void);
	uint8_t readNack (void);
	uint8_t getStatus (void);
	bool statusIs (uint16_t);
};

#endif /* TWI_H_ */
