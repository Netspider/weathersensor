/*****************************************************************************
 
 DS18x20 library
 
 Copyright (C) 2016 Falk Brunner

*****************************************************************************/
 
/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <Falk.Brunner@gmx.de> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return. Falk Brunner
* ----------------------------------------------------------------------------
*/
 
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "onewire.h"
#include "ds18x20.h"

void ds18x20_convert_t(uint8_t parasitic_power)   {

    if (parasitic_power) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            onewire_write_byte(DS18x20_CMD_CONVERT_T);
            ONEWIRE_STRONG_PU_ON
        }
    } else {
        onewire_write_byte(DS18x20_CMD_CONVERT_T);       
    }
}

uint8_t ds18B20_read_temp(int16_t *temperature) {
    int16_t temp;
    uint8_t scratchpad[9];
    
    ds18x20_read_scratchpad(scratchpad);
    if (onewire_crc(scratchpad, 9)) {
        return ONEWIRE_CRC_ERROR;
    }

    temp = ((int16_t)scratchpad[1] << 8) | scratchpad[0];
    // zero undefined LSBs, depending on current resolution
    switch((scratchpad[4] >> 5) & 3) {
        case 0: temp &= ~7; break;    // 9 Bit
        case 1: temp &= ~3; break;    // 10 Bit
        case 2: temp &= ~1; break;    // 11 Bit
    }
    // calculate temperature with 0.1 C resolution using fixed point arithmetic
    // t(0.1C)  = t(1/16C) * 10/16
    *temperature = (temp * 10) >> 4;
    return ONEWIRE_OK;
}

uint8_t ds18S20_read_temp(int16_t *temperature) {
    int16_t temp;
    uint8_t scratchpad[9];
    
    ds18x20_read_scratchpad(scratchpad);
    if (onewire_crc(scratchpad, 9)) {
        return ONEWIRE_CRC_ERROR;
    }

    temp = ((int16_t)scratchpad[1] << 8) | scratchpad[0];
    temp &= ~1;  // clear bit#0
    temp <<= 3;  // x8 -> resolution 1/16 C
    // calculate extended resolution according to data sheet
    // /16 must be omitted, since we are already using a resolution of 1/16 degree C
    temp = temp - 4 + (16-scratchpad[6]);
    // calculate temperature with 0.1 C resolution using fixed point arithmetic
    // t(0.1C)  = t(1/16C) * 10/16
    *temperature = (temp * 10) >> 4;
    return ONEWIRE_OK;
}

void ds18x20_read_scratchpad(uint8_t *buffer) {
    uint8_t i;

    onewire_write_byte(DS18x20_CMD_READ_SCRATCHPAD);
    for (i=0; i<9; i++) {
        buffer[i]=onewire_read_byte();
    }
}

void ds18S20_write_scratchpad(int8_t tl, int8_t th) {

    onewire_write_byte(DS18x20_CMD_WRITE_SCRATCHPAD);
    onewire_write_byte(th);
    onewire_write_byte(tl);
}

void ds18B20_write_scratchpad(int8_t tl, int8_t th, uint8_t adc_resolution) {
    uint8_t cfg;

    onewire_write_byte(DS18x20_CMD_WRITE_SCRATCHPAD);
    onewire_write_byte(th);
    onewire_write_byte(tl);
    switch(adc_resolution) {
        case  9: cfg = 0x00; break;
        case 10: cfg = 0x20; break;
        case 11: cfg = 0x40; break;
        default: cfg = 0x60; break;        // 12 bit
    }
    onewire_write_byte(cfg);
}

void ds18x20_copy_scratchpad(uint8_t parasitic_power) {

    if (parasitic_power) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            onewire_write_byte(DS18x20_CMD_COPY_SCRATCHPAD);
            ONEWIRE_STRONG_PU_ON
        }
    } else {
        onewire_write_byte(DS18x20_CMD_COPY_SCRATCHPAD);
    }
    _delay_ms(10);
    ONEWIRE_STRONG_PU_OFF
}

void ds18x20_recall_E2(void) {
    onewire_write_byte(DS18x20_CMD_RECALL_E2);
    _delay_ms(1);
}

uint8_t ds18x20_read_power_supply(void) {
    onewire_write_byte(DS18x20_CMD_READ_POWER_SUPPLY);
    return !onewire_read_bit();
}
