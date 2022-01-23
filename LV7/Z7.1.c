#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"


void print(char *str);

int main()
{
    XGpio switches;
    int n, data, address;

    XGpio_Initialize(&switches, XPAR_DIP_SWITCHES_8BITS_DEVICE_ID);
    XGpio_SetDataDirection(&switches, 1, 0b00000000);

    while(1)
    {
    	n = XGpio_DiscreteRead(&switches,1);

    	data = XUartLite_RecvByte(XPAR_RS232_UART_1_BASEADDR);

    	address = XPAR_MICRON_RAM_MEM0_BASEADDR + n;

    	Xil_Out32LE(address, data);

    	xil_printf("Na memorijskoj adresi 0x%x spremljen je podatak %d\r\n", address, data);
    }

    return 0;
}