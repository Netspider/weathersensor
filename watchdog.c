/*
  AVR Sleeping Library
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

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "watchdog.h"


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// From http://www.atmel.com/dyn/resources/prod_documents/doc2586.pdf
// * Registers
//
// MCUSR – MCU Status Register
// 0 0 0 0 WDRF BORF EXTRF PORF
//
// • Bit 3 – WDRF: Watchdog Reset Flag
// This bit is set if a Watchdog Reset occurs. The bit is reset by a Power-on Reset, or by writing a
// logic zero to the flag.
//
// WDTCR – Watchdog Timer Control Register
// WDIF WDIE WDP3 WDCE WDE WDP2 WDP1 WDP0
//
// • Bit 7 – WDIF: Watchdog Timeout Interrupt Flag
// This bit is set when a time-out occurs in the Watchdog Timer and the Watchdog Timer is configured
// for interrupt. WDIF is cleared by hardware when executing the corresponding interrupt
// handling vector. Alternatively, WDIF is cleared by writing a logic one to the flag. When the I-bit in
// SREG and WDIE are set, the Watchdog Time-out Interrupt is executed.
// • Bit 6 – WDIE: Watchdog Timeout Interrupt Enable
// When this bit is written to one, WDE is cleared, and the I-bit in the Status Register is set, the
// Watchdog Time-out Interrupt is enabled. In this mode the corresponding interrupt is executed
// instead of a reset if a timeout in the Watchdog Timer occurs.
// If WDE is set, WDIE is automatically cleared by hardware when a time-out occurs. This is useful
// for keeping the Watchdog Reset security while using the interrupt. After the WDIE bit is cleared,
// the next time-out will generate a reset. To avoid the Watchdog Reset, WDIE must be set after
// each interrupt.
//
// See Table 8-2
//
// • Bit 4 – WDCE: Watchdog Change Enable
// This bit must be set when the WDE bit is written to logic zero. Otherwise, the Watchdog will not
// be disabled. Once written to one, hardware will clear this bit after four clock cycles. Refer to the
// description of the WDE bit for a Watchdog disable procedure. This bit must also be set when
// changing the prescaler bits. See “Timed Sequences for Changing the Configuration of the
// Watchdog Timer” on page 45.
// • Bit 3 – WDE: Watchdog Enable
// When the WDE is written to logic one, the Watchdog Timer is enabled, and if the WDE is written
// to logic zero, the Watchdog Timer function is disabled. WDE can only be cleared if the WDCE bit
// has logic level one. To disable an enabled Watchdog Timer, the following procedure must be
// followed:
// 1. In the same operation, write a logic one to WDCE and WDE. A logic one must be written
// to WDE even though it is set to one before the disable operation starts.
// 2. Within the next four clock cycles, write a logic 0 to WDE. This disables the Watchdog.
// (further notes on safety level specific logic - see PDF)
// • Bits 5, 2:0 – WDP[3:0]: Watchdog Timer Prescaler 3, 2, 1, and 0
// The WDP[3:0] bits determine the Watchdog Timer prescaling when the Watchdog Timer is
// enabled. The different prescaling values and their corresponding Timeout Periods are shown in
// See Table 8-3.

// Watchdog timeout values
// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
// 6=1sec, 7=2sec, 8=4sec, 9=8sec
// From http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void watchdog_init(uint8_t ii)
{
	// The prescale value is held in bits 5,2,1,0
	// This block moves ii into these bits
	uint8_t bb;
	if (ii > 9 ) ii = 9;
	bb = ii & 7;
	if (ii > 7) bb |= _BV(WDP3);
	bb |= _BV(WDCE);
	
	// Reset the watchdog reset flag
	cbi(MCUSR,WDRF);
	// step 1: Start timed sequence
	WDTCR |= _BV(WDCE) | _BV(WDE);
	// step 2: Set new watchdog timeout value
	WDTCR = bb;
	// generate interrupt on watchdog timeout
	sbi(WDTCR,WDIE);
}

// wait for waitTime * the configured watchdog timer
// From http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void watchdog_sleep(uint16_t waitTime)
{
  // Calculate the delay time
  uint16_t waitCounter = 0;
  wdt_reset();
  while (waitCounter < waitTime)
  {
    cbi(ADCSRA, ADEN); // Switch Analog to Digital converter OFF 
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode
    sleep_mode(); // System sleeps here
    sbi(ADCSRA, ADEN);  // Switch Analog to Digital converter ON
    waitCounter++;
  }
}

void watchdog_sleepPCINT0(void)
{
  wdt_reset();
  cbi(ADCSRA,ADEN); // Switch Analog to Digital converter OFF 
  sbi(PCMSK,PCINT0); // Enable PCINT0 interrupt
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode
  sleep_enable();
  sbi(GIFR,PCIF); // reset old Pin Change interrupt
  sbi(GIMSK,PCIE); // enable Pin Change interrupts
  sleep_cpu(); // System sleeps here
  cli();
  cbi(GIMSK,PCIE);
  sleep_disable();
  sei();
  sbi(ADCSRA,ADEN);  // Switch Analog to Digital converter ON
}

ISR(WDT_vect)
{
  // Don't do anything here but we must include this
  // block of code otherwise the interrupt calls an
  // uninitialized interrupt handler.
}

ISR(PCINT0_vect) {}

