/*
  KW9010 - AVR libary for emulating the KW9010 sender protocol
  Copyright (c) 2015 Ronny Lindner. All right reserved.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef KW9010_H_
#define KW9010_H_

//#include <avr/io.h>

#define DDR_SENSOR   DDRB
#define PORT_SENSOR  PORTB
#define PIN_SENSOR   PINB
#define SENSOR       PB3

void kw9010_init(void);
void kw9010_send(float temperature, float humidity, bool battery_ok, uint8_t id, uint8_t channel);

uint8_t _state;
uint16_t _timeDummy;
uint16_t _timeSync;
uint16_t _timeZero;
uint16_t _timeOne;
uint8_t _repeatCount;

void _kw9010_sendRaw(uint8_t data[], uint8_t numBits) {
uint8_t _kw9010_generateInternalID(uint8_t id, uint8_t channel);
uint8_t _kw9010_generateChecksum(uint8_t data[], uint8_t numBits);
inline void _kw9010_sendBit(int time1, int time2);
inline void _kw9010_sendSync();
inline void _kw9010_send0();
inline void _kw9010_send1();

#endif /* KW9010_H_ */
