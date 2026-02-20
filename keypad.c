#include <LPC21xx.h>        // LPC21xx register definitions
#include "KeyPdDefines.h"   // Row, Column and LUT definitions

/*----------------------------------------------------
  KeyPdInit()

  Initializes keypad rows as OUTPUT
  Columns are assumed to be INPUT (default state)

  Rows  : P1.16 – P1.19 (R0–R3)
  Columns: P1.20 – P1.23 (C0–C3)
----------------------------------------------------*/
void KeyPdInit(void)
{
    // Set row pins as OUTPUT
    IODIR1 |= ((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));

    // Initialize all rows to LOW (0)
    IOCLR1 = ((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));
}


/*----------------------------------------------------
  ColStat()

  Checks if any key is pressed.

  Returns:
    1 ? No key pressed
    0 ? Some key is pressed
----------------------------------------------------*/
u8 ColStat(void)
{
    // Read column pins P1.20–P1.23
    // If all are HIGH (0x0F), no key is pressed
    if(((IOPIN1 >> 20) & 0x0F) == 0x0F)
        return 1;   // No key pressed
    else
        return 0;   // Key pressed
}


/*----------------------------------------------------
  KeyVal()

  Scans keypad and returns pressed key value.
  Uses row scanning method.
----------------------------------------------------*/
u8 KeyVal(void)
{
    char row_val = 0, col_val = 0;

    /* -------- Check Row 0 -------- */
    IOCLR1 = (1<<R0);                               // Make R0 LOW
    IOSET1 = ((1<<R1)|(1<<R2)|(1<<R3));              // Other rows HIGH

    if(((IOPIN1 >> 20) & 0x0F) != 0x0F)              // If any column LOW
    {
        row_val = 0;
        goto colcheck;
    }

    /* -------- Check Row 1 -------- */
    IOCLR1 = (1<<R1);
    IOSET1 = ((1<<R0)|(1<<R2)|(1<<R3));

    if(((IOPIN1 >> 20) & 0x0F) != 0x0F)
    {
        row_val = 1;
        goto colcheck;
    }

    /* -------- Check Row 2 -------- */
    IOCLR1 = (1<<R2);
    IOSET1 = ((1<<R0)|(1<<R1)|(1<<R3));

    if(((IOPIN1 >> 20) & 0x0F) != 0x0F)
    {
        row_val = 2;
        goto colcheck;
    }

    /* -------- Check Row 3 -------- */
    IOCLR1 = (1<<R3);
    IOSET1 = ((1<<R0)|(1<<R1)|(1<<R2));

    if(((IOPIN1 >> 20) & 0x0F) != 0x0F)
        row_val = 3;

colcheck:

    /* -------- Detect Column -------- */
    if(((IOPIN1 >> C0) & 1) == 0)
        col_val = 0;
    else if(((IOPIN1 >> C1) & 1) == 0)
        col_val = 1;
    else if(((IOPIN1 >> C2) & 1) == 0)
        col_val = 2;
    else
        col_val = 3;

    // Reset all rows to LOW after scanning
    IOCLR1 = ((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));

    // Return key value using Look-Up Table
    return (LUT[row_val][col_val]);
}