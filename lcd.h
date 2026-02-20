#include"types.h"
void InitLCD(void);
void CmdLCD(u8 cmd);
void DispLCD(u8 val);
void CharLCD(u8 dat);
void StrLCD(u8 *ptr);
void IntLCD(s32 num);
void Degree(void);
