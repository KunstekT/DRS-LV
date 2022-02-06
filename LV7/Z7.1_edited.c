#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"


void print(char *str);

int main()
{
    XGpio switches;

    XGpio_Initialize(&switches, XPAR_DIP_SWITCHES_8BITS_DEVICE_ID);
    XGpio_SetDataDirection(&switches, 1, 0b00000000);

    while(1)
    {
        writeToAddressSpecifiedBySwitches(switches);
    }

    return 0;
}

void writeToAddressSpecifiedBySwitches(XGpio switches){
        int n, data, address;

        // n - broj adresa koliko se odstupa od XPAR_RS232_UART_1_BASEADDR, učitano sa switches
    	n = XGpio_DiscreteRead(&switches, 1);
        
		// dohvati podatak s tipkovnice
    	data = XUartLite_RecvByte(XPAR_RS232_UART_1_BASEADDR);

        // address - predstavlja adresu (broj adrese) na kojoj će biti napisan data
    	address = XPAR_MICRON_RAM_MEM0_BASEADDR + n;

        // pisanje data na address nexys 3 uređaja
    	Xil_Out32LE(address, data);

        // Ispisivanje na Tera
    	xil_printf("Na memorijskoj adresi 0x%x spremljen je podatak %d\r\n", address, data);
}