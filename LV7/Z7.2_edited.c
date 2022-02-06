#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"

void print(char *str);

int main() {

	int n;

	spremiPodatkeUneseneTipkovnicomDoPritiskaEnterIZapisiBrojUnesenihZnakovaNaPrvuAdresu();

	n = spremiPodatkeSlijednoNaNexys3(data, n);

	return 0;
}

int spremiPodatkeUneseneTipkovnicomDoPritiskaEnterIZapisiBrojUnesenihZnakovaNaPrvuAdresu(){
	int n=1, data, address;

	while (1) {

		// dohvati podatak s tipkovnice
		data = XUartLite_RecvByte(XPAR_RS232_UART_1_BASEADDR);

		// ako je smislen podatak, spremi na nexys3 na slijedeće 4 adrese (n je counter iskorištenih adresa)
		if ((data >= 65 && data <= 90) || (data >= 97 & data <= 122)) {

			address = XPAR_MICRON_RAM_MEM0_BASEADDR + (n*4);
			Xil_Out32LE(address, data);
			printf("Na memorijskoj adresi 0x%x spremljen je podatak %c\r\n", address, data);
			n++;
		}

		// kada je pritisnut enter, napiši broj učitanih znakova na prvu adresu XPAR_MICRON_RAM_MEM0_BASEADDR (broj znakova n umanjen za 1 zbog znaka enter)
		if (data == 0x0d) {
			address = XPAR_MICRON_RAM_MEM0_BASEADDR;
			Xil_Out32LE(address, n - 1);

			break;
		}
	}
	int brojUnesenihZnakovaTipkovnicom = n - 1;
	return brojUnesenihZnakovaTipkovnicom;
}

void spremiPodatkeSlijednoNaNexys3(int data, int n){

	int i;

	for (i = 0; i <= n; i++) {

        // address - predstavlja adresu (broj adrese) na kojoj će biti napisan data. Uvećava se za 4 svakim korakom for petlje
		address = XPAR_MICRON_RAM_MEM0_BASEADDR + (i*4);

		// data se učitava s dane adrese 'address'
		data = Xil_In32LE(address);

		printf("Na memorijskoj adresi 0x%x spremljen je podatak %d\r\n, zapisan kao %c\r\n", address, data, data);
	}

}