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

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

void watchdog_init(uint8_t ii);
void watchdog_sleep(uint16_t waitTime);
void watchdog_sleepPCINT0(void);

#endif /* WATCHDOG_H_ */
