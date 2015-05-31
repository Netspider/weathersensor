/*
 * ATTINY85 Pins:
 * 
 * 1 RESET?
 * 2 KW9010_DATA
 * 3 AM2302_DATA
 * 4 GND
 * 5 DS18B20_DATA
 * 6 ?
 * 7 ?
 * 8 VCC
 * 
 * aktuell:
 * 1     RESET
 * 2 PB3 AM2302_DATA
 * 3 PB4 LED
 * 4     GND
 * 5 PB0 DS18B20_DATA
 * 6 PB1 KW9010_DATA
 * 7 PB2 ?
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

#include "ds18x20lib.h"
#include "am2302.h"
#include "kw9010.h"


#define led_on      PORT_LED |= (1 << LED);
#define led_off     PORT_LED &= ~(1 << LED);


void blinknum(uint8_t num) {
	for(uint8_t i=num;i>0;i--) {
		led_on;
		_delay_ms(300);
		led_off;
		_delay_ms(300);
	}
}

int main(void)
{
	am2302_init();
	kw9010_init();
	ds1820_init(DS1820);

	DDR_LED |= (1 << LED); // define as output

	led_off;

 	sei();

	while(1)
	{
		uint16_t humidity = 0;
		uint16_t temp = 0;

		_delay_ms(2200);
		uint8_t error = am2302(&humidity, &temp); // get data from am2302
		if (!error)
		{
//			kw9010_send(temp, humidity/10, 1, 0x23, 0);

			blinknum(temp/10);
			_delay_ms(500);
			blinknum(humidity/10);
		}
		else
		{
			led_on;
			_delay_ms(100);
			led_off;
			_delay_ms(100);
			led_on;
			_delay_ms(100);
			led_off;
			_delay_ms(300);
			for(uint8_t errorloop=error; errorloop>0; errorloop--)
			{
				led_on;
				_delay_ms(300);
				led_off;
				_delay_ms(300);
			}
		}

        int16_t temp_outside = ds1820_read_temp(DS1820);
//		kw9010_send(temp_outside, 0, 1, 0x23, 1);
		blinknum(temp_outside/10));
		// wait one second
		_delay_ms(1000);
	}

	return 0;
}
