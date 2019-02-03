/*
 * ATTINY85 Pins:
 * 1     RESET
 * 2 PB3 *_VCC
 * 3 PB4 AM2302_DATA
 * 4     GND
 * 5 PB0 
 * 6 PB1 KW9010_DATA
 * 7 PB2 DS18B20_DATA
 * 8     VCC
 * 
 * Strom-Messung:
 * AM2302   3200 uA (Data - 10k - VCC, Data GND, max)
 * DS18B20  1080 uA (Data - 4k7 - VCC, Data GND, max)
 * FS1000A 12000 uA (Data - VCC)
 * 
 * AM2302   1600 uA (Data - 10k - VCC, Data GND)
 * DS18B20  1080 uA (Data - 4k7 - VCC, Data GND)
 * FS1000A    10 uA (Data - open)
 * 
 * AM2302     51 uA (Data - 10k - VCC, Data open)
 * DS18B20     4 uA (Data - 4k7 - VCC, Data open)
 * FS1000A     0 uA (Data - GND)
 * 
 * ATTINY85 1MHz 5V + AM2302: 1900 uA
 * http://oregonembedded.com/batterycalc.htm
 * 
 * AM2302:  2s 2000uA, 1s 50uA
 * DS18B20: 1s 1080uA, 2s 4uA
 * FS1000A: 2s 10uA, 1s 6000uA ( (12000+0)/2 )
 * (2000+2000+50+1080+4+4+10+10+6000)/3 = 3719uA * 3s + 1900uA (ATTINY)
 * 
 */


#include "main.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "onewire.h"
#include "ds18x20.h"
#include "am2302.h"
#include "kw9010.h"
#include "watchdog.h"

uint8_t tmpDDR = 0;
uint8_t tmpPORT = 0;

inline void vcc_on(void) {
	DDRB = tmpDDR;
	PORTB = tmpPORT;
	DDR_VCC |= (1 << PIN_VCC); // output
	_delay_us(1);
	PORT_VCC |= (1 << PIN_VCC); // HIGH
}

inline void vcc_off(void) {
	tmpDDR = DDRB;
	tmpPORT = PORTB;
	DDRB = 0;
	PORTB = 0;
	PORT_VCC &= ~(1 << PIN_VCC);
	DDR_VCC &= ~(1 << PIN_VCC); //input
}

int main(void)
{
	tmpDDR = DDRB;
	tmpPORT = PORTB;
	watchdog_init(9);
	am2302_init();
	kw9010_init();

 	sei();

	while(1)
	{
		vcc_on();
		uint16_t humidity = 0;
		uint16_t temp = 0;

		uint8_t error = am2302(&humidity, &temp);
		if (!error)
		{
			kw9010_send(temp, humidity/10, 1, ID1, 0);
		}

		onewire_skip_rom();
		ds18S20_convert_t(0); // normal power
		_delay_ms(750);
		onewire_skip_rom();
		int16_t temp_outside;
		int rc = ds18S20_read_temp(&temp_outside);
		if (rc) {
            		// Serial.println(F("CRC error!"));
		} else {
			kw9010_send(temp_outside, 0, 1, ID2, 0);
		}
		vcc_off();
#ifdef DEBUGMODE 
		watchdog_sleep(2); // 16 Sekunden
#else
		watchdog_sleep(10*60/8); // 10 Minuten
#endif
	}
	return 0;
}
