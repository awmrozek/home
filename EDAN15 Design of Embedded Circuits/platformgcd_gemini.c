#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "myip.h"
#include "xil_io.h"

int main()
{
    init_platform();

    print("Hello World\n\r");


    MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR, 0, 17);
    MYIP_mWriteReg(XPAR_MYIP_0_S00_AXI_BASEADDR, 1, 5);
   	int reg0 = MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR, 0);
   	xil_printf("REG: %x\n", reg0);

   	reg0 = MYIP_mReadReg(XPAR_MYIP_0_S00_AXI_BASEADDR, 1);
   	xil_printf("REG: %x\n", reg0);

    cleanup_platform();
    return 0;
}


/*
del IP
Refresh IP Catalog
add ip
connection automation
*/