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

#include "kw9010.h"

#include <util/delay.h>

#define SENSOR_data_out		DDR_SENSOR |= (1 << SENSOR)
#define SENSOR_data_in		DDR_SENSOR &= ~(1 << SENSOR)
#define SENSOR_data_high	PORT_SENSOR |= (1 << SENSOR)
#define SENSOR_data_low		PORT_SENSOR &= ~(1 << SENSOR)
#define SENSOR_is_high		PIN_SENSOR & (1 << SENSOR)
#define SENSOR_is_low		!(PIN_SENSOR & (1 << SENSOR))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

void kw9010_init(void)
{
	return;
}

uint8_t _kw9010_generateInternalID(uint8_t id, uint8_t channel) {
	uint8_t output = 0;
	output |= (id & 0b00111100) << 2;		// iiii0000
	output |= (channel & 0b00000011) << 2;	// iiiicc00
	output |= (id & 0b00000011);			// iiiiccii
	return output;
}

uint8_t _kw9010_generateChecksum(uint8_t data[], uint8_t numBits) {
	uint8_t sum = 0;
	uint8_t curByte = 0;
	uint8_t curBit = 0;
	uint8_t tmpData = 0;
	for(uint8_t i=0; i<numBits; i++) {
		bitWrite(tmpData, curBit%4, bitRead(data[curByte], 7-curBit));
		curBit++;
		if(curBit % 4 == 0) {
			sum += tmpData;
			tmpData = 0;
			if(curBit >= 8) {
				curByte++;
				curBit = 0;
			}
		}
	}
	uint8_t invSum = 0;
	for(uint8_t i=0; i<4; i++) {
		bitWrite(invSum, 3-i, bitRead(sum, i));
	}
	return invSum;
}

void _kw9010_sendSync(void) {
	if( _state )
		SENSOR_data_low;
	else
		SENSOR_data_high;
	_delay_us(_timeSync);
	_state = ! _state;
}

void _kw9010_send0(void) {
	// no need to change state as we would toggle twice
	if( _state )
		SENSOR_data_low;
	else
		SENSOR_data_high;
	_delay_us(_timeDummy);
	if( _state )
		SENSOR_data_high;
	else
		SENSOR_data_low;
	_delay_us(_timeZero);
}

void _kw9010_send1(void) {
	// no need to change state as we would toggle twice
	if( _state )
		SENSOR_data_low;
	else
		SENSOR_data_high;
	_delay_us(_timeDummy);
	if( _state )
		SENSOR_data_high;
	else
		SENSOR_data_low;
	_delay_us(_timeOne);
}

void _kw9010_sendRaw(uint8_t data[], uint8_t numBits) {
	_state = 0;
	SENSOR_data_low;
	// no buffering, saves some RAM
	// hopefully fast enough
	for(uint8_t count = 0; count < _repeatCount; count++) {
		uint8_t curChar = 0;
		uint8_t bitHelper = 128;
		_kw9010_sendSync();
		for(uint8_t curBit = 0; curBit < numBits; curBit++) {
			((data[curChar] & bitHelper) == 0) ? _kw9010_send0() : _kw9010_send1();
			bitHelper = bitHelper >> 1;
			if(bitHelper <= 0) {
				bitHelper = 128;
				curChar++;
			}
		}
	}
	_state = 0;
	SENSOR_data_low;
}

void kw9010_send(int16_t temperature, uint8_t humidity, uint8_t battery_ok, uint8_t id, uint8_t channel) {
  uint8_t data[9] = {0};
  data[0] = _kw9010_generateInternalID(id, channel);
  // battery ok?
  bitWrite(data[1], 7, (uint8_t) ! battery_ok);
  // trend not implemented yet
  bitClear(data[1], 6);
  bitClear(data[1], 5);
  // forced send not implemented yet
  bitClear(data[1], 4);
  // temperature
  if(temperature < 0) {
    // negative temperature flag
    temperature += 4096;
  }
  for(uint8_t i=0; i<4; i++) {
    if(bitRead(temperature, i) != 0) {
      bitSet(data[1], 3-i);
    }
  }
  for(uint8_t i=0; i<8; i++) {
    if(bitRead(temperature, i+4) != 0) {
      bitSet(data[2], 7-i);
    }
  }
  // humidity
  uint8_t tmpHumidity = humidity + 156;
  for(uint8_t i=0; i<8; i++) {
    if(bitRead(tmpHumidity, i) != 0) {
      bitSet(data[3], 7-i);
    }
  }
  // checksum
  data[4] = _kw9010_generateChecksum(data, 32) << 4;

  _kw9010_sendRaw(data, 36);
}
