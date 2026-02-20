#include <LPC21xx.h>      // LPC21xx register definitions
#include "lcd.h"          // LCD functions
#include "lm35.h"         // LM35 temperature sensor functions
#include "uart.h"         // UART communication functions
#include "rtc.h"          // RTC functions
#include "keyPd.h"        // Keypad functions
#include "delay.h"        // Delay functions
#include "data_logger.h"  // Data logger header

/*----------------------------------------------------
  Display RTC Temperature on LCD
----------------------------------------------------*/
void DispRTCTemp()
{
    CmdLCD(0x8A);          // Move cursor to specific LCD position
    CharLCD('T');          // Display 'T'
    CharLCD(':');          // Display ':'
    IntLCD(Read_LM35('C'));// Read LM35 temperature in Celsius & display
    
    CmdLCD(0x48);          // Go to CGRAM location
    Degree();              // Create degree symbol
    
    CmdLCD(0x8E);          // Move cursor
    CharLCD(1);            // Print custom degree symbol
    CharLCD('C');          // Print 'C'
}

/*----------------------------------------------------
  Send Temperature via UART
----------------------------------------------------*/
void DispUARTTemp(void)
{
    UARTTxStr(" Temp: ");           // Print label
    UARTTxU32(Read_LM35('C'));      // Send temperature value
    UARTTxChar(0xB0);               // Degree symbol in ASCII
    UARTTxStr("C @ ");              // Print unit
}

/*----------------------------------------------------
  Send Time via UART
----------------------------------------------------*/
void DisplayUARTTime(u32 hour, u32 minute, u32 second)
{
    // Convert digits to ASCII by adding 48
    UARTTxChar((HOUR/10) + 48);
    UARTTxChar((HOUR%10) + 48);
    UARTTxChar(':');
    UARTTxChar((MIN/10) + 48);
    UARTTxChar((MIN%10) + 48);
    UARTTxChar(':');
    UARTTxChar((SEC/10) + 48);
    UARTTxChar((SEC%10) + 48);
    UARTTxChar(' ');
}

/*----------------------------------------------------
  Send Date via UART
----------------------------------------------------*/
void DisplayUARTDate(u32 date,u32 month,u32 year)
{
    UARTTxChar((date/10)+48);
    UARTTxChar((date%10)+48);
    UARTTxChar('/');
    UARTTxChar((month/10)+48);
    UARTTxChar((month%10)+48);
    UARTTxChar('/');
    UARTTxU32(year);        // Send full year
}

/*----------------------------------------------------
  Display Main Edit Menu on LCD
----------------------------------------------------*/
void LCDDispInfo(void)
{
    CmdLCD(0x01);               // Clear LCD
    delay_ms(2);
    CmdLCD(0x80);               // First line
    StrLCD("1.EDIT TIME INFO");
    CmdLCD(0xC0);               // Second line
    StrLCD("2.EDIT SP 3.EXIT"); 
}

/*----------------------------------------------------
  Display Time/Date Editing Menu
----------------------------------------------------*/
void LCD_Menu(void)
{
    u8 i;
    u8 pos[] = {0x80, 0xC0};   // LCD line positions
    
    // Menu labels
    char menu1[] = {'H','M','S','D'};
    char menu2[] = {'M','Y','D','E'};
    
    CmdLCD(pos[0]);
    for(i=0;i<4;i++)
    {
        IntLCD(i+1);           // Display option number
        CharLCD('.');
        CharLCD(menu1[i]);     // Display label
        CharLCD(' ');
    }

    CmdLCD(pos[1]);
    for(i=0;i<4;i++)
    {
        IntLCD(i+5);
        CharLCD('.');
        CharLCD(menu2[i]);
        CharLCD(' ');
    }
}

/*----------------------------------------------------
  Edit Time and Date Function
----------------------------------------------------*/
void Edit_Time_Date(void)
{
    u8 key;
    s32 H,Mi,S,D,M,Y;
    u8 weekday;

    LCD_Menu();     // Show edit options

    while(1)
    {
        while(ColStat());      
        delay_ms(10);
        key=KeyVal();          // Read key
        while(!ColStat());

        switch(key)
        {
            case 1:     // Edit Hour
                CmdLCD(0x01);
                StrLCD("Enter Hour:");
                H = GetKeypadNumber();
                if(H > 23) H = 23;   // Limit hour
                HOUR=H;
                break;

            case 2:     // Edit Minute
                CmdLCD(0x01);
                StrLCD("Enter Minute:");
                Mi = GetKeypadNumber();
                if(Mi > 59) Mi = 59;
                MIN=Mi;
                break;

            case 3:     // Edit Second
                CmdLCD(0x01);
                StrLCD("Enter Second:");
                S = GetKeypadNumber();
                if(S > 59) S = 59;
                SEC=S;
                break;

            case 4:     // Edit Date
                CmdLCD(0x01);
                StrLCD("Enter Date:");
                D = GetKeypadNumber();
                if(D < 1 || D > 31) D = 1;
                DOM=D;
                break;

            case 5:     // Edit Month
                CmdLCD(0x01);
                StrLCD("Enter Month:");
                M = GetKeypadNumber();
                if(M < 1 || M > 12) M = 1;
                MONTH=M;
                break;

            case 6:     // Edit Year
                CmdLCD(0x01);
                StrLCD("Enter Year:");
                Y = GetKeypadNumber();
                if(Y < 2000) Y = 2000;
                YEAR=Y;
                break;

            case 7:     // Update Day Automatically
                weekday = GetDayFromDate();
                SetRTCDay(weekday);
                CmdLCD(0x01);
                StrLCD("Day Updated");
                delay_ms(500);
                break;

            case 8:     // Save & Exit
                CmdLCD(0x01);
                StrLCD("Saved");
                delay_ms(800);
                return;
        }
        LCD_Menu();     // Show menu again
    }
}

/*----------------------------------------------------
  Take Multi-Digit Number from Keypad
----------------------------------------------------*/
u32 GetKeypadNumber(void)
{
    u32 num = 0;
    u8 key;

    while(1)
    {
        while(ColStat());
        delay_ms(10);
        key = KeyVal();
        while(!ColStat());

        if(key <= 9)          // If digit pressed
        {
            num = (num * 10) + key;  // Form number
            IntLCD(key);             // Display digit
        }
        else if(key == 10)    // Backspace
        {
            if(num > 0)
            {
                num = num / 10;      
                CmdLCD(0x10);        
                CharLCD(' ');        
                CmdLCD(0x10);        
            }
        }
        else if(key == 11)    // Enter
        {
            return num;
        }
    }
}

/*----------------------------------------------------
  Select Day from Menu
----------------------------------------------------*/
u8 GetDayFromDate(void)
{
    u8 key;

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("0-S 1-M 2-T 3-W");
    CmdLCD(0xC0);
    StrLCD("4-T 5-F 6-S");

    while(1)
    {
        while(ColStat());
        delay_ms(10);
        key=KeyVal();
        while(!ColStat());

        if(key>=0 && key<=6)
            return key;      // Return selected day
    }
}

/*----------------------------------------------------
  Edit Set Point Temperature
----------------------------------------------------*/
void Edit_SP(void)
{
    extern u32 SP;      // Access global SP

    CmdLCD(0x01);
    while(!ColStat());  
    delay_ms(200);   

    StrLCD("Enter SP:");
    SP=GetKeypadNumber();   // Get new SP value

    if(SP>150) SP=150;      // Safety limit

    CmdLCD(0x01);
    StrLCD("SP Saved");
    delay_ms(1000);
}