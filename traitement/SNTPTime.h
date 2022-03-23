/*
 * SNTPTime.h - ESP8266-specific SNTPClock library
 * Copyright (c) 2015 Peter Dobler. All rights reserved.
 * This file is part of the esp8266 core for Arduino environment.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef SNTPTIME_H
#define SNTPTIME_H

#include <Arduino.h>
extern "C" {
#include "user_interface.h"
}

//marc comment
//struct timeval
//{
//  time_t tv_sec;
//  long   tv_usec;
//};

// time structure represents time A.D. (e.g. year: 2015)
//In file included from C:\marc\developpement\vmc et tempo bus can\traitement\SNTPTime.cpp:22:
//SNTPTime.h:35:8: error: redefinition of 'struct tm'
//   35 | struct tm {
//      |        ^~
//In file included from C:\Users\mireille\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\cores\esp8266/HardwareSerial.h:31,
//                 from C:\Users\mireille\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\cores\esp8266/Arduino.h:288,
//                 from C:\marc\developpement\vmc et tempo bus can\traitement\SNTPTime.h:22,
//                 from C:\marc\developpement\vmc et tempo bus can\traitement\SNTPTime.cpp:22:
//c:\users\mireille\appdata\local\arduino15\packages\esp8266\tools\xtensa-lx106-elf-gcc\3.0.4-gcc10.3-1757bed\xtensa-lx106-elf\include\time.h:37:8: note: previous definition of 'struct tm'
//   37 | struct tm
//      |        ^~
//In file included from C:\marc\developpement\vmc et tempo bus can\traitement\SNTPClock.h:23,
//                 from C:\marc\developpement\vmc et tempo bus can\traitement\SNTPClock.cpp:23:
//SNTPTime.h:35:8: error: redefinition of 'struct tm'
//   35 | struct tm {

//struct tm {
//  int tm_sec;  /* seconds,          range 0 to 59  */
//  int tm_min;  /* minutes,          range 0 to 59  */
//  int tm_hour; /* hours,            range 0 to 23  */
//  int tm_mday; /* day of the month, range 1 to 31  */
//  int tm_mon;  /* month,            range 0 to 11  */
//  int tm_year; /* number of years   since 1900     */
//  int tm_wday; /* day of the week,  range 0 to 6   */ //sunday = 0
//  int tm_yday; /* day in the year,  range 0 to 365 */
//  int tm_isdst;/* daylight saving time             */ //no=0, yes>0
//};

// the one and only time instance
static tm Time;

#define MONTH_START 1 /* month in strings is in range 1..12 */
#define DIFF1900TO1970 2208988800UL

const char mon_name[12][4] PROGMEM = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char wday_name[7][4] PROGMEM = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const int  dayOfMonth[]    PROGMEM = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// TODO
//clock_t clock(void);  // in CLOCKS_PER_SEC
//double difftime (time_t end, time_t beginning);
time_t mktime (struct tm * timeptr);
time_t time_1 (time_t * timer = NULL);  //time doublon avec C:\Users\mireille\AppData\Local\
//Arduino15\packages\esp8266\hardware\esp8266\2.5.2\cores\esp8266\time.cpp

char * asctime (const struct tm * timeptr);
char * ctime (const time_t * timer);
struct tm * gmtime (const time_t * timer);
struct tm * localtime (const ulong * timer, long timezone = 0, int daylight = 0); //secsSince1900 -> tm

tm *   mkTmStruct(ulong secsSince1900);
tm *   splittime(ulong secsSince1900);
int    gettimeofday(void * tp, void * tzp);
int    clock_gettime(clockid_t clock_id, timespec *tp);
void   addTimeStruct(tm * t1, tm * t2);

tm *   localtime(const ulong secsSince1900, long timezone, int daylight);
ulong  removeDayLight(ulong secsSince1900);
int    compareTime(tm * t1, tm * t2);
void   configTime(ulong timezone, int daylight);
void   addTimeSeconds(tm * t1, ulong sec2);

#endif // SNTPTIME_H
