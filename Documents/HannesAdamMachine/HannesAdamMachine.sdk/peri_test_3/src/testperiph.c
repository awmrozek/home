#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "xgpio.h"
#include "gpio_header.h"
#include "xtmrctr.h"
#include "tmrctr_header.h"

typedef unsigned int ui;

#include "xuartlite_l.h"

unsigned getnum() {
	char srb=0;
	unsigned num=0;

	// skip non digits
	while(srb < '0' || srb > '9') srb=XUartLite_RecvByte(STDIN_BASEADDRESS);

	// read all digits
	while(srb >= '0' && srb <= '9') {
		num=num*10+(srb-'0');
		srb=XUartLite_RecvByte(STDIN_BASEADDRESS);
	};
	return num;
}

ui gcd1 (ui a, ui b) {
	ui c;
	if (a < b)
		c = a;
	else
		c = b;
	while (c != 0)
	{
		if ( a % c == 0 && b % c == 0 )
			break;
		else
			c--;
	}
	return c;
}

ui gcd2 (ui a, ui b) {
	while (a != b) {
		if (b > a)
			b -= a;
		else
			a -= b;
	}

	return a;
}

int main() 
{
   Xil_ICacheEnable();
   Xil_DCacheEnable();
   
   // Timer
   XTmrCtr timer;
   if(XTmrCtr_Initialize(&timer, XPAR_TMRCTR_0_DEVICE_ID) != XST_SUCCESS){
	   xil_printf("Timer did not initialize\n");
	   while(1);
   }




   // Input part
   int fstNum = 34; // = getnum();
   int sndNum = 23; // = getnum();

   /*Use the timer IP via code for measuring the clock cycles required by your
   solution. Make sure you can record the computation time rather than the
   time required to read the data from the UART. You should attempt to
   split the program in an �input� phase in which you read all the numbers,
   a �compute� phase in which you carry out the GCD algorithm and an
   �output� phase.*/
   // Starta timer

   ui i, j, g1, g2;
   u32 time_val;
   XTmrCtr_Start(&timer, 0);
   for (i = 1; i < 19; ++i) {
	   for (j = 1; j < 10; ++j) {
		   g1 = gcd1(i, j);
		   g2 = gcd2(i, j);
	   }
   }
   xil_printf("Stop\n");
   XTmrCtr_Stop(&timer, 0);
   time_val = XTmrCtr_GetValue(&timer, 0);
   // Stoppa timer
   xil_printf("%d %d, Time: %d\n", g1, g2, time_val);

   print("---Exiting main---\n\r");
   Xil_DCacheDisable();
   Xil_ICacheDisable();
   return 0;
}