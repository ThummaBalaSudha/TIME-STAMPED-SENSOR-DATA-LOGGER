#include <LPC21xx.h>      // LPC21xx register definitions
#include "macros.h"       // READBIT macro definition
#include "pin_connect.h"  // Pin configuration function
#include "types.h"        // Custom data types (u32, s8, f32 etc.)

/*----------------------------------------------------
  InitUART()

  Initializes UART0 for serial communication.
  Configuration:
    - 8-bit data
    - 1 stop bit
    - No parity
    - Baud rate set using DLL & DLM
----------------------------------------------------*/
void InitUART(void)
{
    // Configure P0.0 as TXD0 (UART0 Transmit)
    CfgPinFunc(0,0,1);

    // If receiving needed:
    // CfgPinFunc(0,1,1); // P0.1 as RXD0

    U0LCR = 0x03;      // 8-bit word length, 1 stop bit, no parity
    U0LCR |= (1<<7);   // Set DLAB = 1 to access DLL & DLM registers

    U0DLL = 97;        // Set baud rate (depends on PCLK)
    U0DLM = 0;

    U0LCR &= ~(1<<7);  // Clear DLAB (normal operation mode)
}

/*----------------------------------------------------
  UARTRxChar()

  Receives one character from UART.
  Waits until data is available.
----------------------------------------------------*/
s8 UARTRxChar(void)
{
    while(!READBIT(U0LSR,0));   // Wait until RDR (Receiver Data Ready) bit = 1
    return U0RBR;               // Return received byte
}

/*----------------------------------------------------
  UARTTxChar()

  Transmits one character via UART.
----------------------------------------------------*/
void UARTTxChar(s8 ch)
{
    U0THR = ch;                 // Load character into transmit register
    while(!READBIT(U0LSR,6));   // Wait until THR Empty (transmission complete)
}

/*----------------------------------------------------
  UARTTxStr()

  Transmits a null-terminated string.
----------------------------------------------------*/
void UARTTxStr(s8 *ptr)
{
    while(*ptr)                 // Loop until NULL character
        UARTTxChar(*ptr++);     // Send character and move pointer
}

/*----------------------------------------------------
  UARTTxU32()

  Transmits an unsigned 32-bit integer.
  Converts number to ASCII before sending.
----------------------------------------------------*/
void UARTTxU32(u32 num)
{
    u8 a[10];   // Temporary array to store digits
    s8 i = 0;

    if(num == 0)
        UARTTxChar('0');
    else
    {
        // Extract digits in reverse order
        while(num > 0)
        {
            a[i++] = (num % 10) + 48;  // Convert digit to ASCII
            num = num / 10;
        }

        // Send digits in correct order
        for(--i; i >= 0; i--)
            UARTTxChar(a[i]);
    }
}

/*----------------------------------------------------
  UARTTxF32()

  Transmits a floating-point number.
  Displays 6 digits after decimal point.
----------------------------------------------------*/
void UARTTxF32(f32 fnum)
{
    u32 num, i;

    // Handle negative numbers
    if(fnum < 0)
    {
        UARTTxChar('-');
        fnum = -fnum;
    }

    num = fnum;          // Extract integer part
    UARTTxU32(num);      // Send integer part
    UARTTxChar('.');     // Decimal point

    // Send 6 digits after decimal
    for(i = 0; i < 6; i++)
    {
        fnum = (fnum - num) * 10;
        num = fnum;
        UARTTxChar(num + 48);
    }
}