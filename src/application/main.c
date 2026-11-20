
#include <LPC21xx.h>
#include "types.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "delay.h"
#include "kpm.h"
#include "rtc.h"
#include "uart.h"
#include "atm.h"
#include "intrrupt.h"

int main(void)
{
  /*  InitLCD();
    InitKPM();
    InitRTC();
    UART0_Init();
	  
	
	  RTC_SET_DATE(27, 10, 2026);
    RTC_SET_TIME(0, 30, 16);
    InitEINT3();

    welcome();

    while (1)
    {
        CheckInterruptRequest();
        welcome();
    }*/
}
    /* Set initial time for testing only */


/*
#include <LPC21xx.h>
#include "types.h"
#include "defines.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "kpm.h"

u32 keyV;

 
u32 rsec, rmin, rhrs;
u32 rdate, rmonth, ryear;

int main(void)
{
    InitLCD();
    InitKPM();

    while(1)
    {
        CmdLCD(CLEAR_LCD);
        StrLCD("Press a key:");

        keyV = KeyScan();

        CmdLCD(GOTO_LINE2_POS0);
        CharLCD((u8)keyV);

        while(ColScan() == 0);
    }
}


#include <lpc21xx.h>
#include "types.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "delay.h"
#include "kpm.h"
#include "rtc.h"
#include "uart.h"
#include "atm.h"

 
u32 rsec, rmin, rhrs;
u32 rdate, rmonth, ryear;

int main(void)
{
    InitLCD();
    InitKPM();
    InitRTC();
    UART0_Init();

    welcome();

    while(1)
    {
         ATM functions handle the user interaction 
    }
}




#include <lpc21xx.h>
#include "delay.h"
#include "types.h"
#include "lcd_atm_defines.h"
#include "lcd_atm.h"
#include "rtc.h"
#include "uart.h"

u32 rsec, rmin, rhrs;
u32 rdate, rmonth, ryear;

int main(void)
{
    InitLCD();
    InitRTC();
    UART0_Init();

    RTC_SET_TIME(10, 5, 3);
    RTC_SET_DATE(15, 10, 2026);

    while(1)
    {
        RTC_READ_TIME();
        RTC_READ_DATE();

         Display time on LCD 
        CmdLCD(CLR_DSP);
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("TIME:");

        U32LCD(rhrs);
        StrLCD(":");
        U32LCD(rmin);
        StrLCD(":");
        U32LCD(rsec);

         Display time on UART 
        U0_TxU32(rhrs);
        UART0_Tx(':');
        U0_TxU32(rmin);
        UART0_Tx(':');
        U0_TxU32(rsec);
        UART0_Tx('\r');
        UART0_Tx('\n');

        delay_ms(1000);
    }
}
*/

