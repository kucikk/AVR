/*
 * twi.h
 *
 * Created: 22. 11. 2014 10:09:35
 *  Author: Tomáš Kučera <me@kucikk.cz> http://kucikk.cz
 */ 


#ifndef TWI_H_
#define TWI_H_

class TWI {
private:
	uint32_t cpu;
	uint32_t clock;
public:
	TWI (uint32_t, uint32_t);
	void init (void);
	void start (void);
	void stop (void);
	void write (uint8_t);
	uint8_t readACK (void);
	uint8_t readNACK (void);
	uint8_t getStatus (void);
	bool statusIs (uint16_t);
};

#endif /* TWI_H_ */
