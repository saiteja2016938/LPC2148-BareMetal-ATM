//intrrupt.h



#ifndef INTRRUPT_H
#define INTRRUPT_H

#include "types.h"

extern volatile u32 rtc_setup_requested;

void InitEINT3(void);
void eint3_isr(void) __irq;

#endif
