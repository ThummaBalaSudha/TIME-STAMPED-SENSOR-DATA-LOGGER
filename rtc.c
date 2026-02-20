#include <LPC21xx.H>      // LPC21xx register definitions
#include "rtc_defines.h"  // RTC register macro definitions
#include "types.h"        // Custom data types (u32, s32 etc.)
#include "lcd.h"          // LCD display functions

/*----------------------------------------------------
  Array storing names of days (3-letter format)
  Index: 0=Sunday ... 6=Saturday
----------------------------------------------------*/
char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

/*----------------------------------------------------
  RTC_Init()
  Initializes the Real Time Clock module.

  Steps:
  1. Reset RTC
  2. Configure prescaler (if required)
  3. Enable RTC
----------------------------------------------------*/
void RTC_Init(void) 
{
    CCR = RTC_RESET;   // Disable and reset RTC

#ifdef _LPC2148
    // For LPC2148: Enable RTC & select clock source
    CCR = RTC_ENABLE | RTC_CLKSRC;  

#else
    // For other LPC21xx controllers:
    // Configure prescaler values to generate 1 second tick
    PREINT  = PREINT_VAL;  
    PREFRAC = PREFRAC_VAL;

    CCR = RTC_ENABLE;   // Enable RTC
#endif
}

/*----------------------------------------------------
  GetRTCTimeInfo()
  Reads current time from RTC registers

  hour   -> pointer to store hour (0–23)
  minute -> pointer to store minute (0–59)
  second -> pointer to store second (0–59)
----------------------------------------------------*/
void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second)
{
    *hour   = HOUR;   // Read hour register
    *minute = MIN;    // Read minute register
    *second = SEC;    // Read second register
}

/*----------------------------------------------------
  DisplayRTCTime()
  Displays time in HH:MM:SS format on LCD
----------------------------------------------------*/
void DisplayRTCTime(u32 hour, u32 minute, u32 second)
{
    CmdLCD(0x80);   // Move cursor to first row, first column

    // Convert numeric digits to ASCII by adding 48
    CharLCD((hour/10) + 48);
    CharLCD((hour%10) + 48);
    CharLCD(':');

    CharLCD((minute/10) + 48);
    CharLCD((minute%10) + 48);
    CharLCD(':');

    CharLCD((second/10) + 48);
    CharLCD((second%10) + 48);
}

/*----------------------------------------------------
  GetRTCDateInfo()
  Reads current date from RTC registers

  date  -> Day of month (1–31)
  month -> Month (1–12)
  year  -> Full year (4 digits)
----------------------------------------------------*/
void GetRTCDateInfo(s32 *date, s32 *month, s32 *year)
{
    *date  = DOM;     // Day of month register
    *month = MONTH;   // Month register
    *year  = YEAR;    // Year register
}

/*----------------------------------------------------
  DisplayRTCDate()
  Displays date in DD/MM/YYYY format on LCD
----------------------------------------------------*/
void DisplayRTCDate(u32 date, u32 month, u32 year)
{
    CmdLCD(0xC0);   // Move cursor to second row

    CharLCD((date/10) + 48);
    CharLCD((date%10) + 48);
    CharLCD('/');

    CharLCD((month/10) + 48);
    CharLCD((month%10) + 48);
    CharLCD('/');

    // Display 4-digit year manually
    CharLCD((year/1000) + 48);
    CharLCD(((year/100)%10) + 48);
    CharLCD(((year/10)%10) + 48);
    CharLCD((year%10) + 48);
}

/*----------------------------------------------------
  SetRTCTimeInfo()
  Sets RTC time registers

  hour   -> 0–23
  minute -> 0–59
  second -> 0–59
----------------------------------------------------*/
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second)
{
    HOUR = hour;   // Set hour register
    MIN  = minute; // Set minute register
    SEC  = second; // Set second register
}

/*----------------------------------------------------
  SetRTCDateInfo()
  Sets RTC date registers

  date  -> 1–31
  month -> 1–12
  year  -> 4-digit year
----------------------------------------------------*/
void SetRTCDateInfo(u32 date, u32 month, u32 year)
{
    DOM   = date;   // Set day of month
    MONTH = month;  // Set month
    YEAR  = year;   // Set year
}

/*----------------------------------------------------
  GetRTCDay()
  Reads current day of week

  day -> pointer (0=Sunday ... 6=Saturday)
----------------------------------------------------*/
void GetRTCDay(s32 *day)
{
    *day = DOW;   // Read Day Of Week register
}

/*----------------------------------------------------
  DisplayRTCDay()
  Displays day name (SUN, MON, etc.) on LCD
----------------------------------------------------*/
void DisplayRTCDay(u32 dow)
{
    CmdLCD(0xCB);       // Move cursor to specific position
    StrLCD(week[dow]);  // Display corresponding day string
}

/*----------------------------------------------------
  SetRTCDay()
  Sets Day Of Week register

  day -> 0=Sunday ... 6=Saturday
----------------------------------------------------*/
void SetRTCDay(u32 day)
{
    DOW = day;   // Write day to register
}