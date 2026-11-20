//lcd_test.c
#include "lcd_defines.h"
#include "lcd.h"
#include "delay.h"
const u8 cgramLUT[24]=
{
	0x1d,0x05,0x05,0x1F,0x14,0x14,0x17,0x00,
	0x15,0x15,0x15,0x1f,0x04,0x04,0x04,0x00,
	0x04,0x0a,0x04,0x1f,0x04,0x0a,0x11,0x00
};	
main()
{
	InitLCD();
	CharLCD('A');
	delay_ms(1000);
	StrLCD(" V25HE12-ARM ");
	CmdLCD(GOTO_LINE2_POS0);
	U32LCD(1234567890);
	delay_ms(1000);
	CmdLCD(CLEAR_LCD);
	S32LCD(-12345678);
	delay_ms(1000);
	CmdLCD(GOTO_LINE2_POS0);
	F32LCD(123.456789,6);
	delay_ms(1000);
	CmdLCD(CLEAR_LCD);
	BuildCGRAM(cgramLUT,24);
	CharLCD(0);
	delay_ms(1000);
	CharLCD(1);
	delay_ms(1000);
	CharLCD(2);
	delay_ms(1000);
	
	while(1);
}
