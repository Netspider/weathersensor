/*
 * main.c
 *
 * Created on: 13.03.2013
 *     Author: Pascal Gollor
 *        web: http://www.pgollor.de
 *
 * Dieses Werk ist unter einer Creative Commons Lizenz vom Typ
 * Namensnennung - Nicht-kommerziell - Weitergabe unter gleichen Bedingungen 3.0 Deutschland zugänglich.
 * Um eine Kopie dieser Lizenz einzusehen, konsultieren Sie
 * http://creativecommons.org/licenses/by-nc-sa/3.0/de/ oder wenden Sie sich
 * brieflich an Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 *
 * testet with:
 * - avr-gcc 4.3.4
 * - Atmega8 @ 8 MHz
 */


#include "main.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "am2302.h"


#define led_on      PORT_LED |= (1 << LED);
#define led_off     PORT_LED &= ~(1 << LED);


int main(void)
{
	am2302_init();
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
			led_on;
			for(uint8_t temploop=humidity/10; temploop>0; temploop--)
				_delay_ms(100);
			led_off;
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

		// wait one second
		_delay_ms(1000);
	}

	return 0;
}
