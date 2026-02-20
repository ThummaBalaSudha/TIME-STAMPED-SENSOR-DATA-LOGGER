#include "types.h"
#define FN1 0 
#define SW 4     
#define BUZ 25

void DisplayUARTTime(u32, u32, u32);
void DisplayUARTDate(u32, u32, u32);

void DispRTCTemp(void);
void DispUARTTemp(void);

void LCDDispInfo(void);
void LCD_Menu(void);

void Edit_Time_Date(void);
void Edit_SP(void);

u32 GetKeypadNumber(void);
u8 GetDayFromDate(void);
