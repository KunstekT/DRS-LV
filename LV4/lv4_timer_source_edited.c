#include <stdio.h>
#include "xparameters.h"
#include "xtmrctr.h"	// timer
#include "xintc.h"		// interrupt
#include "xgpio.h"		// io

//definiranje konstante za redni broj timera koji se koristi (unutar timer komponente postoje dva timera)
#define TIMER_CNTR_0 0

// Postaviti vrijednost konstante RESET_VALUE
const RESET_VALUE = 49999998

int TmrCtrIntrInit(XIntc* IntcInstancePtr,
 XTmrCtr* TmrInstancePtr,
 u16 TimerDeviceId,
 u16 IntcDeviceId,
 u16 IntrSourceId,
 u8 TmrCtrNumber);

void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);

// Deklarirati varijable interrupt controllera, timer controllera i io (ledice)
XIntc InterruptController;
XTmrCtr TimerCounterInst;
XGpio leds;

// Deklarirati globalnu varijablu TimerExpired
volatile int TimerExpired;

int main(void)
{
	print("-- Start of the program! --\r\n");

	int Status;

	// Inicijalizirati kontroler za komunikaciju s LE diodama
	XGpio_initialize(&leds, XPAR_LEDS_8BITS_DEVICE_ID);	
	XGpio_SetDataDirection(&leds, 1, 0x00000000);

	// Poziv funkcije za inicijalizaciju timera i upravljača prekidima
	Status = TmrCtrIntrInit(&InterruptController,
							&TimerCounterInst,
							XPAR_DELAY_DEVICE_ID,
							XPAR_INTC_0_DEVICE_ID,
							XPAR_INTC_0_TMRCTR_0_VEC_ID,
							TIMER_CNTR_0);

	if (Status != XST_SUCCESS){
		return XST_FAILURE;
	}

	// Pokrenuti timer
	XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);

	while (1) {

	}

	print("-- End of the program! --\r\n");
	return XST_SUCCESS;
}

/********************************************************************/
/**
* Inicijalizacija timera i upravljača prekidima.
* Funkcija prima sljedeće parametre:
*
* @paramIntcInstancePtr - pokazivač na varijablu tipa XIntc,
* @paramTmrCtrInstancePtr - pokazivač na varijablu tipa XTmrCtr,
* @paramTimerDeviceId - vrijednost konstante XPAR_<TmrCtr_instance>_DEVICE_ID iz datoteke xparameters.h,
* @paramIntcDeviceId - vrijednost konstante XPAR_<Intc_instance>_DEVICE_ID iz datoteke xparameters.h,
* @paramIntrSourceId - vrijednost konstante XPAR_<INTC_instance>_<TmrCtr_instance>_INTERRUPT_INTR iz datoteke xparameters.h,
* @paramTmrCtrNumber - redni broj timera koji se inicijalizira.
*
* @returnXST_SUCCESS ako je inicijalizacija uspješna, a u suprotno funkcija vraća XST_FAILURE
*
*********************************************************************/
int TmrCtrIntrInit(XIntc* IntcInstancePtr,
 XTmrCtr* TmrCtrInstancePtr,
 u16 TimerDeviceId,
 u16 IntcDeviceId,
 u16 IntrSourceId,
 u8 TmrCtrNumber)
{
	int Status;

	print("Init STARTED\r\n");

	// Inicijalizirati timer
	Status = XTmrCtr_Initialize(TmrCtrInstancePtr, TimerDeviceId);
	if (Status != XST_SUCCESS) {
		print("Timer Initialize FAILED\r\n");
		return XST_FAILURE;
	}

	print("Timer Initialize SUCCESS\r\n");

	//****Inicijalizirati interrupt controller
	Status = XIntc_Initialize(&IntcInstancePtr, IntcDeviceId);
	if (Status != XST_SUCCESS) {
		print("Intc Initialize FAILED\r\n");
		return XST_FAILURE;
	}

	print("Intc Initialize SUCCESS\r\n");

	/*
	 * Povezivanje upravljača prekida s rukovateljem prekida koji se
	 * poziva kada se dogodi prekid. Rukovatelj prekida obavlja
	 * specifične zadatke vezane za rukovanje prekidima.
	 */
	Status = XIntc_Connect(IntcInstancePtr, IntrSourceId,
	 (XInterruptHandler)XTmrCtr_InterruptHandler,
	 (void *)TmrCtrInstancePtr);
	if (Status != XST_SUCCESS) {
		print("Intc Connect FAILED\r\n");
		return XST_FAILURE;
	}
	print("Intc Connect SUCCESS\r\n");

	// Postaviti mod rada upravljača prekida
	Status = XIntc_Start(&IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		print("Intc Start FAILED\r\n");
		return XST_FAILURE;
	}
	print("Intc Start SUCCESS\r\n");

	// Omogućiti rad upravljača prekidima
	XIntc_Enable(&IntcInstancePtr, IntrSourceId);

	//Omogućavanje microblaze prekida.
	microblaze_enable_interrupts();

	/*
	 * Postavljanje prekidne rutine koja će biti pozvana kada se dogodi prekid
	 * od strane timera. Kao parametri predaju se pokazivač na komponentu za
	 * koju se postavlja prekidna rutina, naziv prekidne rutine (funkcije)
	 * i pointer na timer, kako bi prekidna rutina mogla pristupiti timeru.
	 */
	XTmrCtr_SetHandler(TmrCtrInstancePtr,
	 TimerCounterHandler,
	 TmrCtrInstancePtr);

	// Postaviti postavke timera: Omogućiti prekide, omogućiti auto reload, odabrati brojanje prema dolje
	XTmrCtr_SetOptions(TmrCtrInstancePtr, TmrCtrNumber, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION | XTC_DOWN_COUNT_OPTION);

	// Postaviti početnu vrijednost timera
	XTmrCtr_SetResetValue(TmrCtrInstancePtr, TmrCtrNumber, RESET_VALUE);

	print("Init FINISHED\r\n");
	return XST_SUCCESS;
}

/*
 * Prekidna rutina koja se poziva kada timer generira prekid.
 * Funkcija prima pokazivač na void parametar CallBackRef
 * koji se cast-a na pokazivač tipa XTmrCtr.
 * Ovaj parametar je napravljen kako bi se pokazao način na
 * koji se unutar prekidne rutine može pristupiti timer
 * komponenti i njenim funkcijama.
*/
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
{
	print("Interrupt Handler!\r\n");

	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;

	// Deklarirati varijable timeMS (int) i uCount (uint)
	int timeMS;
	uint uCount;

	// Brojati prekide i vrijednost prikazati na LE diodama razvojnog sustava
	TimerExpired++;
	XGpio_DiscreteWrite(&leds ,1 , TimerExpired);

	// Dohvatiti vrijednost brojača, izračunati vrijeme u ms i vrijednost poslati preko UART-a
	uCount = XTmrCtr_GetValue(InstancePtr, TmrCtrNumber);
	timeMS = (uCount + 2)/ 50000;
	xil_printf("Elapsed time: %d ms\tcount: %d\r\n", timeMS, uCount);
}
