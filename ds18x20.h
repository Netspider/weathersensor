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
 
/**
\mainpage

 \par ds18x20.h - DS18x20 library

 \author Falk Brunner

 \version 1.00
 
 \par License:
 \subpage LICENSE "Beerware License"
 
 \par Files:
    \subpage ds18x20.h \n
    \subpage ds18x20.c \n

 \par Developed on AVR plattform, but can be used with any other
      AVR-Studio 4.18, WinAVR20100110 (avr-gcc 4.3.3) \n
      Atomic access must be adapted to other plattforms!

 \par Example:
 \include ./onewire_demo.ino
 \page LICENSE Beerware License
 \include ./beerware.txt
 \page DS18X20.H ds18x20.h
 \include ./ds18x20.h
 \page DS18X20.C ds18x20.c
 \include ./ds18x20.c
*/

#ifndef DS18x20_H_
#define DS18x20_H_

#include <stdint.h>

/** \defgroup DS18x20_COMMANDS DS18x20 COMMANDS
  command codes for DS18x20
*/
/*@{*/
#define DS18x20_CMD_SKIP_ROM          0xCC
#define DS18x20_CMD_CONVERT_T         0x44
#define DS18x20_CMD_READ_SCRATCHPAD   0xBE
#define DS18x20_CMD_WRITE_SCRATCHPAD  0x4E
#define DS18x20_CMD_COPY_SCRATCHPAD   0x48
#define DS18x20_CMD_RECALL_E2         0xB8
#define DS18x20_CMD_READ_POWER_SUPPLY 0xB4
/*@}*/

/** \defgroup DS18x20_ID DS18x20 IDs
  ID codes for DS18x20
*/
/*@{*/
#define DS18B20_ID 0x28
#define DS18S20_ID 0x10
/*@}*/

/** \defgroup DS18X20_FUNCTIONS DS18X20 FUNCTIONS 
  * standard functions
  */
/*@{*/ 

/**
 \brief start temperature conversion
 \param parasitic_power set true if device has parasitic power supply
 \return none
 */   
void  ds18x20_convert_t(uint8_t parasitic_power);

#define ds18B20_convert_t(x) ds18x20_convert_t(x)
#define ds18S20_convert_t(x) ds18x20_convert_t(x)

/**
 \brief Read temperature from DS18B20 (9-12 bit resolution)
 \param *temperature pointer to temperature variable in 1/10 C (fixed point)
 \return error code
 \return ONEWIRE_OK no error
 \return ONEWIRE_CRC_ERROR CRC error when reading temperature
 */   

uint8_t ds18B20_read_temp(int16_t *temperature);

/**
 \brief Read temperature from DS18S20 (9 bit + enhanced resolution, effective 12 bits)
 \param *temperature pointer to temperature variable in 1/10 C (fixed point)
 \return error code
 \return ONEWIRE_OK no error
 \return ONEWIRE_CRC_ERROR CRC error when reading temperature
 */   

uint8_t ds18S20_read_temp(int16_t *temperature);

/**
 \brief Read complete scratchpad of DS18x20 (9 bytes)
 \param *buffer pointer to data array
 \return none
 */   

void ds18x20_read_scratchpad(uint8_t *buffer);

#define ds18B20_read_scratchpad(x) ds18x20_read_scratchpad(x)
#define ds18S20_read_scratchpad(x) ds18x20_read_scratchpad(x)

/**
 \brief write tl and th of DS18S20
 \param tl lower temperature limit (1 C resolution)
 \param th upper temperature limit (1 C resolution)
 \return none
 */   

void ds18S20_write_scratchpad(int8_t tl, int8_t th);

/**
 \brief write tl, th and configuration of DS18B20
 \param tl lower temperature limit (1 C resolution)
 \param th upper temperature limit (1 C resolution)
 \param adc_resolution ADC resolution in bits (9-12)
 \return none
 */   

void ds18B20_write_scratchpad(int8_t tl, int8_t th, uint8_t adc_resolution);

/**
 \brief copy scratchpad to EEPROM, busy waiting (10ms),
 \brief parasitic power switching is completely handled inside the function, no need for further user action
 \param parasitic_power set true if device has parasitic power supply
 \return none
 */   

void ds18x20_copy_scratchpad(uint8_t parasitic_power);

#define ds18B20_copy_scratchpad(x) ds18x20_copy_scratchpad(x);
#define ds18S20_copy_scratchpad(x) ds18x20_copy_scratchpad(x);

/**
 \brief copy EEPROM to scratchpad, busy waiting (1ms)
 \param none
 \return none
 */   

void ds18x20_recall_E2(void);
#define ds18B20_recall_E2(x) ds18x20_recall_E2(x)
#define ds18S20_recall_E2(x) ds18x20_recall_E2(x)

/**
 \brief read power supply
 \param none
 \return power mode
 \return 0: normal power
 \return 1: parasitic power
 */   

uint8_t ds18x20_read_power_supply(void);

#define ds18B20_read_power_supply(x) ds18x20_read_power_supply(x)
#define ds18S20_read_power_supply(x) ds18x20_read_power_supply(x)

/*@}*/

#endif
