//intrrupt.c


#include <LPC21xx.h>
#include "types.h"
#include "intrrupt.h"

#define EINT3_VIC_CHN 17

volatile u32 rtc_setup_requested = 0;
void RTC_SetupMode(void);
void eint3_isr(void) __irq;

void InitEINT3(void)
{
    /* 1. Configure P0.20 as EINT3 */
    PINSEL1 &= ~(3U << 8);
    PINSEL1 |=  (3U << 8);

    /* 2. Configure EINT3 as IRQ */
    VICIntSelect &= ~(1U << EINT3_VIC_CHN);

    /* 3. Enable EINT3 in VIC */
    VICIntEnable = (1U << EINT3_VIC_CHN);

    /* 4. Configure EINT3 as vectored IRQ in slot 0 */
    VICVectCntl0 = (1U << 5) | EINT3_VIC_CHN;

    /* 5. Load ISR address into vector slot 0 */
    VICVectAddr0 = (u32)eint3_isr;

    /* 6. Configure EINT3 for edge-triggered operation */
    EXTMODE |= (1U << 3);

    /* 7. Configure falling-edge trigger */
    EXTPOLAR &= ~(1U << 3);

    /* 8. Clear any pending EINT3 interrupt */
    EXTINT = (1U << 3);
}

void eint3_isr(void) __irq
{
    /* Request RTC setup; handle it outside the ISR */
  // rtc_setup_requested = 1;
 // CheckInterruptRequest();
    RTC_SetupMode();

    /* Clear EINT3 peripheral interrupt */
    EXTINT = (1U << 3);

    /* Acknowledge VIC */
    VICVectAddr = 0;
	
}
