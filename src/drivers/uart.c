// uart.c
#include <lpc21xx.h>
#include "types.h"
#include "uart_defines.h"


void UART0_Init(void)
{
    // cfg p0.0 ,p0.1 pins as 
	// TXD & RXD pins respectively
	PINSEL0&=~((3 << 2) | ( 3 << 0));
	PINSEL0|= TXD0_PIN_FUNC|RXD0_PIN_FUNC;
	 //cfg UxLCR for 8N1 ,&DLAB sets
	 U0LCR = (( 1 << DLAB_BIT)|(_8BIT << WORD_LEN_SEL_BITS));

	// cfg baudrate

	U0DLL = DIVISOR;
	U0DLM = DIVISOR >> 8;
	// reset / clear DLAB bit
	U0LCR &= ~ (1 << DLAB_BIT);

}

void UART0_Tx(u8 sDat)
{
// write to UxTHR, serialization begins 
U0THR = sDat;
// wait until serialization out compleates 
while(((U0LSR >> TEMT_BIT)&1)==0);

}

u8 UART0_Rx(void)
{

   // wait until reception status observed 
   while(((U0LSR>>DR_BIT)&1)==0);
   // read & return recvd byte 
   return U0RBR;

 }

 void U0_TxStr(s8 *s)
 {

       while(*s)
	       UART0_Tx(*s++);


}

void U0_TxU32(u32 n)
{
     s32 i=0;		u8 a[10];

	 if(n==0)
	 {
	     UART0_Tx('0');
	 }
	 else
	 {
	      while(n>0)
		  {
		         a[i++]=(n%10)+48;
				 n/=10;

	      }
		  for(--i;i>=0;i--)
		  {
		      UART0_Tx(a[i]);
		  }

	}
}

void U0_TxS32(s32 n)
{


    if(n < 0)
	{
	     UART0_Tx('-');
		 n=-n;

	}
	U0_TxU32(n);
}

void U0_TxF32(f32 fn,u8 nDp)
{

   u32 n,i;

        if(fn < 0.0)
		{

		UART0_Tx('-');
		fn = -fn;
		}

    n = fn;
	U0_TxU32(n);
	UART0_Tx('.');
	 for(i=0; i < nDp ; i++)
	 {
	      fn= (fn-n)*10;
		  n=fn;

	UART0_Tx(n+48);
	}
}

s8 * U0_RxStr(void)
{
     static s8 rBuff[100];

	 s8 t, i=0;
	 while(1)
	 {
	     t = UART0_Rx();
		 rBuff[i]=t;
		 UART0_Tx(t);
		 if((t=='\n')||(t=='\r'))
		 {
		     rBuff[i]='\0';
			 break;

		 }
		 i++;
    }
      U0_TxStr("\n\r");

	return rBuff;

}

