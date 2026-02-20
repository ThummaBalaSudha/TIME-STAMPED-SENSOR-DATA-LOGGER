#include <lpc21xx.h>       // LPC21xx register definitions

#include "pin_connect.h"   // Pin configuration functions
#include "delay.h"         // Delay functions
#include "uart.h"          // UART functions
#include "rtc.h"           // RTC functions
#include "adc_defines.h"   // ADC definitions
#include "adc.h"           // ADC functions
#include "lm35.h"          // LM35 temperature sensor functions
#include "lcd.h"           // LCD functions
#include "keyPd.h"         // Keypad functions
#include "data_logger.h"   // Data logger functions

// Global Variables
u32 SP = 40;               // Set Point temperature (default 40°C)
u8 key;                    // To store keypad key value
s32 hour, min, sec;        // Time variables
s32 date, month, year;     // Date variables
s32 day;                   // Day variable
static u8 flag = 0;        // Used to avoid multiple execution at sec = 59

int main()
{
    int edit_flag = 0;     // Used to control menu mode

    // -------- Initialization Section --------
    InitUART();            // Initialize UART
    RTC_Init();            // Initialize RTC
    Init_ADC(CH0);         // Initialize ADC Channel 0
    InitLCD();             // Initialize LCD
    KeyPdInit();           // Initialize Keypad
    
    CfgPinFunc(0, SW, FN1);   // Configure Switch pin
    CfgPinFunc(0, BUZ, FN1);  // Configure Buzzer pin
    
    IODIR0 &= ~(1<<SW);    // Make Switch as input
    IODIR0 |= (1<<BUZ);    // Make Buzzer as output
    
    // -------- Set Initial RTC Time & Date --------
    SetRTCTimeInfo(11,51,1);      // Set time: 11:51:01
    SetRTCDateInfo(03,01,2026);   // Set date: 03/01/2026
    SetRTCDay(1);                 // Set day

    while (1) 
    {
        // -------- Display RTC Time on LCD --------
        GetRTCTimeInfo(&hour,&min,&sec);
        DisplayRTCTime(hour,min,sec);
        
        // -------- Display RTC Date on LCD --------
        GetRTCDateInfo(&date,&month,&year);
        DisplayRTCDate(date,month,year);
        
        // -------- Display Day --------
        GetRTCDay(&day);
        DisplayRTCDay(day);
        
        // -------- Display Temperature --------
        DispRTCTemp();
        
        // -------- Every 59th Second Action --------
        if(sec == 59 && flag == 0)
        {
            flag = 1;   // Prevent repeated execution

            // If temperature is below Set Point
            if(Read_LM35('C') < SP)
            {
                IOCLR0 = (1<<BUZ);      // Turn OFF buzzer
                DispUARTTemp();         // Send temperature via UART
            
                GetRTCTimeInfo(&hour,&min,&sec);
                DisplayUARTTime(hour,min,sec);
            
                GetRTCDateInfo(&date,&month,&year);
                DisplayUARTDate(date,month,year);
            
                UARTTxStr("\n\r"); 
            }
            
            // If temperature is equal or above Set Point
            if(Read_LM35('C') >= SP)
            {
                IOSET0 = (1<<BUZ);      // Turn ON buzzer (Alert)
                DispUARTTemp();
            
                GetRTCTimeInfo(&hour,&min,&sec);
                DisplayUARTTime(hour,min,sec);
            
                GetRTCDateInfo(&date,&month,&year);
                DisplayUARTDate(date,month,year);
                
                UARTTxStr(" - OVER TEMP!\n\r");
            }   
        }

        // Reset flag when second changes
        if(sec != 59)
            flag = 0;

        // -------- Switch Press Detection --------
        if(((IOPIN0>>SW)&1) == 0)   // If switch pressed (active low)
        {
            delay_ms(10);           // Debounce delay
            while(((IOPIN0>>SW)&1) == 0);   // Wait until switch release

            edit_flag = 1;          // Enter menu mode
            LCDDispInfo();          // Show menu (1.Edit 2.SP 3.Exit)

            // -------- MENU LOOP --------
            while(edit_flag)
            {
                while(ColStat());       // Wait for key press
                delay_ms(10);
                key = KeyVal();         // Read key
                while(!ColStat());      // Wait for key release

                // Option 1: Edit Time/Date
                if(key == 1)
                {
                    UARTTxStr(" ***Time Editing Mode Activated***\n\r");
                    Edit_Time_Date();   // Press 8 to return
                    LCDDispInfo();      // Show menu again
                }

                // Option 2: Edit Set Point
                else if(key == 2)
                {
                    UARTTxStr(" ***SP Editing Mode Activated***\n\r");
                    Edit_SP();
                    LCDDispInfo();
                }

                // Option 3: Exit Menu
                else if(key == 3)
                {
                    edit_flag = 0;      // Exit menu
                    UARTTxStr(" ***Editing Mode DeActivated***\n\r");
                    CmdLCD(0x01);       // Clear LCD
                    delay_ms(10);
                }
            }
        }
    }
}