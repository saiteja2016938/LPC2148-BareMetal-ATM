// uart.h

#include "types.h"
// primary drivers

void UART0_Init(void);
void UART0_Tx(u8 sDat);
u8 UART0_Rx(void);



// secondary drivers

void U0_TxStr(s8 *);
void U0_TxU32(u32 );
void U0_TxS32(s32 );
void U0_TxF32(f32,u8);
s8 * U0_RxStr(void);

