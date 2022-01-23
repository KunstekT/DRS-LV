#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"

void print(char *str);

int main() {
	XGpio switches;
	int n = 1, data, address;

	XGpio_Initialize(&switches, XPAR_DIP_SWITCHES_8BITS_DEVICE_ID);
	XGpio_SetDataDirection(&switches, 1, 0b00000000);

	while (1) {
		data = XUartLite_RecvByte(XPAR_RS232_UART_1_BASEADDR);

		if ((data >= 65 && data <= 90) || (data >= 97 & data <= 122)) {
			address = XPAR_MICRON_RAM_MEM0_BASEADDR + (n*4);
			Xil_Out32LE(address, data);
			printf("Na memorijskoj adresi 0x%x spremljen je podatak %c\r\n", address, data);
			n++;
		}
		if (data == 0x0d) {
			address = XPAR_MICRON_RAM_MEM0_BASEADDR;
			Xil_Out32LE(address, n - 1);

			break;
		}
	}
	int i;
	for (i = 0; i < n; i++) {
		address = XPAR_MICRON_RAM_MEM0_BASEADDR + (i*4);
		data = Xil_In32LE(address);

		printf("Na memorijskoj adresi 0x%x spremljen je podatak %d\r\n, zapisan kao %c\r\n", address, data, data);
	}

	return 0;
}