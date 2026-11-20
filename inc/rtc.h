//rtc.h
#ifndef RTC_H
#define RTC_H

#include "types.h"

extern u32 rsec, rmin, rhrs;
extern u32 rdate, rmonth, ryear;

void InitRTC(void);

void RTC_SET_TIME(u32 sec, u32 min, u32 hrs);
void RTC_SET_DATE(u32 date, u32 month, u32 year);

void RTC_READ_TIME(void);
void RTC_READ_DATE(void);

#endif

