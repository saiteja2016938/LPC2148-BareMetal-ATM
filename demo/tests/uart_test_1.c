#include<lpc21xx.h>

void uart0_Init(void)
{
   PINSEL0|= 0x05;
   U0LCR = 0x83;
   U0DLL = 0x61;
   U0DLM = 0X00;
   U0LCR = 0x03;
}


void uart0_tx(char ch)
{

   while(!(U0LSR & (1 << 5)));
   U0THR = ch;

}


char uart0_rx(void)
{
    while(!(U0LSR & 1));
	return U0RBR;
}

void uart0_tx_string(char *s)
{
   while(*s) uart0_tx(*s++);

}
					   		
int main(void)
{


     uart0_Init();
	 uart0_tx_string("UART LPC2129 Ready....\n");
	 while(1)
	 {
	     char ch= uart0_rx();
		 uart0_tx(ch);		  //echo back
	  
	 }

}

   





   
