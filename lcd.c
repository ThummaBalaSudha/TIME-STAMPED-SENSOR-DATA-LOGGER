#include <LPC21xx.h>   // LPC21xx register definitions
#include "delay.h"     // delay_ms function
#include "types.h"     // Custom data types (u8, s32 etc.)
#include "macros.h"    // WRITEBYTE macro
#include "lcd.h"       // LCD function declarations

#define LCD_DAT 0xFF   // LCD data lines connected to P0.16 – P0.23
#define RS  12         // P0.12 ? Register Select
#define RW  13         // P0.13 ? Read/Write
#define EN  14         // P0.14 ? Enable

/*----------------------------------------------------
  InitLCD()

  Initializes 16x2 LCD in 8-bit mode.
----------------------------------------------------*/
void InitLCD(void)
{
    // Configure LCD pins as output
    IODIR0 |= ((LCD_DAT << 16) | (1<<RS) | (1<<RW) | (1<<EN));

    delay_ms(20);       // Initial LCD power-on delay

    // LCD initialization sequence (8-bit mode)
    CmdLCD(0x30);
    delay_ms(5);

    CmdLCD(0x30);
    delay_ms(1);

    CmdLCD(0x30);
    delay_ms(1);

    CmdLCD(0x38);  // 8-bit mode, 2 lines, 5x7 matrix
    CmdLCD(0x10);  // Cursor move
    CmdLCD(0x01);  // Clear display
    CmdLCD(0x06);  // Entry mode (increment cursor)
    CmdLCD(0x0F);  // Display ON, cursor ON, blinking ON
}

/*----------------------------------------------------
  CmdLCD()

  Sends command to LCD.
  RS = 0 ? Command register
----------------------------------------------------*/
void CmdLCD(u8 cmd)
{
    IOCLR0 = (1<<RS);   // RS = 0 (command mode)
    DispLCD(cmd);       // Send command
}

/*----------------------------------------------------
  DispLCD()

  Sends data/command byte to LCD.
----------------------------------------------------*/
void DispLCD(u8 val)
{
    IOCLR0 = (1<<RW);         // RW = 0 (Write mode)

    WRITEBYTE(IOPIN0,16,val); // Send 8-bit data to P0.16–P0.23

    IOSET0 = (1<<EN);         // Enable = 1
    delay_ms(2);

    IOCLR0 = (1<<EN);         // Enable = 0 (Latch data)
    delay_ms(5);
}

/*----------------------------------------------------
  CharLCD()

  Displays one character on LCD.
  RS = 1 ? Data register
----------------------------------------------------*/
void CharLCD(u8 dat)
{
    IOSET0 = (1<<RS);   // RS = 1 (data mode)
    DispLCD(dat);       // Send character
}

/*----------------------------------------------------
  StrLCD()

  Displays string on LCD.
----------------------------------------------------*/
void StrLCD(u8 *ptr)
{
    while(*ptr != '\0')   // Until NULL character
        CharLCD(*ptr++);  // Send each character
}

/*----------------------------------------------------
  IntLCD()

  Displays signed integer on LCD.
----------------------------------------------------*/
void IntLCD(s32 num)
{
    u8 a[10];   // Temporary array to store digits
    s8 i = 0;

    if(num == 0)
        CharLCD('0');
    else
    {
        if(num < 0)
        {
            num = -num;
            CharLCD('-');
        }

        // Convert number to ASCII (reverse order)
        while(num > 0)
        {
            a[i++] = (num % 10) + 48;
            num = num / 10;
        }

        // Print digits in correct order
        for(--i; i >= 0; i--)
            CharLCD(a[i]);
    }
}

/*----------------------------------------------------
  Degree()

  Writes custom degree symbol pattern.
  (Used for temperature display like 25°C)
----------------------------------------------------*/
void Degree(void)
{
    u8 i;
    u8 a[] = {0x07,0x05,0x07,0x00,0x00,0x00,0x00,0x00};

    // Write custom character pattern (8 bytes)
    for(i = 0; i < 8; i++)
    {
        CharLCD(a[i]);
    }
}