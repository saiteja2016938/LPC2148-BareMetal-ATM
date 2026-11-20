#include <LPC21xx.h>
#include "RTC_defines.h"
#include "types.h"
#include "rtc.h"

u32 rsec, rmin, rhrs;
u32 rdate, rmonth, ryear;

void InitRTC(void)
{
    /* Stop the RTC counter while configuring it */
    CCR = 0;

    /* Prescaler values for a 15 MHz RTC clock source.
       Confirm these values against your board clock configuration. */
    PREINT  = 0x000001C8;
    PREFRAC = 0x000061C0;

    /* Clear pending RTC interrupt flags */
    ILR = (1 << ILR_BIT_RTCCIF) |
          (1 << ILR_BIT_RTCALF);

    /* Start RTC */
    CCR = (1 << CCR_BIT_CLKEN);
}

void RTC_SET_TIME(u32 sec, u32 min, u32 hrs)
{
    SEC  = sec;
    MIN  = min;
    HOUR = hrs;
}

void RTC_SET_DATE(u32 date, u32 month, u32 year)
{
    DOM   = date;
    MONTH = month;
    YEAR  = year;
}

void RTC_READ_TIME(void)
{
    rsec = SEC;
    rmin = MIN;
    rhrs = HOUR;
}

void RTC_READ_DATE(void)
{
    rdate  = DOM;
    rmonth = MONTH;
    ryear  = YEAR;
}
