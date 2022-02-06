#include "stdio.h"
#include "xil_exception.h"
#include "xstatus.h"
#include "xparameters.h"
#include "xps2.h"
#include "xintc.h"
#include "xgpio.h"

// Definirati konstante PS2_DEVICE_ID, INTC_DEVICE_ID i INTR_ID

#define PS2_DEVICE_ID XPAR_PS2_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_INTC_0_DEVICE_ID
#define INTR_ID XPAR_INTC_0_PS2_0_VEC_ID

/*************************** Definiranje makroa *****************************/
#define printf xil_printf

/************************** Prototipi funkcija ******************************/

int Ps2IntrExample(XIntc* IntcInstPtr, XPs2* Ps2InstPtr, u16 Ps2DeviceId, u8 Ps2IntrId);

static void Ps2IntrHandler(void *CallBackRef, u32 Event, u32 EventData);

static int Ps2SetupIntrSystem(XIntc* IntcInstPtr, XPs2 *Ps2Ptr, u8 IntrId);

// Deklarirati varijable Ps2Inst i IntcInst

XPs2 Ps2Inst;
XIntc IntcInst;

XGpio leds;
volatile int counter = 0;
volatile u8 buffer = 0x00;
volatile u8 ledsValue = 0x01;
volatile int flag = 0;

//******************************************************************************************************//

//Varijable koje se koriste u prekidnoj rutini
volatile static int RxDataRecv = FALSE; 	/* Flag to indicate Receive Data */
volatile static int RxError = FALSE; 		/* Flag to indicate Receive Error */
volatile static int RxOverFlow = FALSE; 	/* Flag to indicate Receive Overflow */
volatile static int TxDataSent = FALSE; 	/* Flag to indicate Tx Data sent */
volatile static int TxNoAck = FALSE;    	/* Flag to indicate Tx No Ack */
volatile static int TimeOut = FALSE;   		/* Flag to indicate Watchdog Timeout */
volatile static int TxNumBytes = 0;			/* Number of bytes transmitted */
volatile static int RxNumBytes = 0;     	/* Number of bytes received */

// Deklarirati varijablu RxBuffer
u8 RxBuffer;

int main(void)
{
	int Status;

// Pozvati funkciju Ps2IntrExample
	Status = Ps2IntrExample(&IntcInst,&Ps2Inst,PS2_DEVICE_ID,INTR_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;

}

/****************************************************************************/
/**
*
* Ova funkcija je primjer inicijalizacije i komunikacije s tipkovnicom u interrupt načinu rada.
* APIs.
*
*
* @param	IntcInstPtr je pokazivač na varijablu tipa XIntc.
* @param	Ps2InstPtr je pokazivač na varijablu tipa XPs2.
* @param	Ps2DeviceId je device id XPS PS2 kontrolera iz datoteke xparameters.h.
* @param	Ps2IntrId je vrijednost konstante XPAR_<INTC_instance>_<PS2_instance>_VEC_ID iz datoteke xparameters.h.
* @return
*		- XST_SUCCESS ako je funkcija završio uspješno.
*		- XST_FAILURE ako je funkcija nije završila uspješno.
*
* @note		Funkcija nikada neće vratiti XST_SUCCESS ukoliko se ne implementira prekid beskonačne petlje.
****************************************************************************/
int Ps2IntrExample(XIntc* IntcInstPtr, XPs2* Ps2InstPtr,
			u16 Ps2DeviceId, u8 Ps2IntrId)
{
	int Status;
	XPs2_Config *ConfigPtr;

	// Dohvatiti konfiguraciju PS2 sučelja
	ConfigPtr = XPs2_LookupConfig(Ps2DeviceId);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}

	// Inicijalizirati PS2 kontroler
	XPs2_CfgInitialize(Ps2InstPtr,ConfigPtr,ConfigPtr->BaseAddress);

	XGpio_Initialize(&leds,XPAR_LEDS_8BITS_DEVICE_ID);
	XGpio_SetDataDirection(&leds,1,0x00000000);
	XGpio_DiscreteWrite(&leds,1,ledsValue);

	//Self Test PS/2 uređaja
	Status = XPs2_SelfTest(Ps2InstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Pozvati funkciju Ps2SetupIntrSystem
	Status = Ps2SetupIntrSystem(IntcInstPtr,Ps2InstPtr,Ps2IntrId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Postavljanje prekidne rutine
	XPs2_SetHandler(&Ps2Inst, (XPs2_Handler)Ps2IntrHandler, &Ps2Inst);

	// Omogućiti prekide za prijem podataka i globalno omogućiti prekide u XPS PS2 kontroleru
	XPs2_IntrEnable(Ps2InstPtr,XPS2_IPIXR_ALL);
	XPs2_IntrGlobalEnable(&Ps2Inst);

	printf("\r\n Press Keys on the keyboard \r\n");

	//Beskonačna petlja
	while(1)
	{

	}

	//Onemogućavanje prekida za prijem i globalno onemogućavanje prekida u XPS PS2 kontroleru
	//XPs2_IntrDisable(&Ps2Inst, XPS2_IPIXR_ALL);
	//XPs2_IntrGlobalDisable(&Ps2Inst);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
* Prekidna rutina koja se poziva od strane procesora u slučajevima kada XPS PS2
* kontroler generira jedan od sljedećih prekida:
*	- Primanje podataka 	(Receive Data Interrupt)
*	- Primanje pogreške 	(Receive Error Interrupt)
*	- Primanje prelijevanja	(Receive Overflow Interrupt)
*	- Slanje podataka		(Transmit Data Interrupt)
*	- Ne primanje ACK		(Transmit No ACK Interrupt)
*	- Watchdog timeout		(Watchdog Timeout Interrupt)
*
* @param 	CallBackRef je callback referenca koja se prosljeđuje u prekidnu
*			rutinu.
* @param	IntrMask je maska koja govori koji je događaj izazvao prekid.
*			U datoteci xps2_l.h se nalaze konstante XPS2_IPIXR_* kojima se
*			objašnjavaju događaji koji su izazvali prekid:
*		- XPS2_IPIXR_RX_FULL za Primanje podataka
*		- XPS2_IPIXR_RX_ERR za Primanje pogreške
*		- XPS2_IPIXR_RX_OVF za Primanje prelijevanja
*		- XPS2_IPIXR_TX_ACK za Slanje podataka
*		- XPS2_IPIXR_TX_NOACK za Ne primanje ACK
*		- XPS2_IPIXR_WDT_TOUT za Watchdog timeout
* @param 	ByteCount sadrži broj bytova koji su primljeni ili poslani u
*			trenutku poziva.
*
* @return	None.
*
* @note		None.*
******************************************************************************/
static void Ps2IntrHandler(void *CallBackRef, u32 IntrMask, u32 ByteCount)
{
	// Primiti 1 byte podataka putem PS2 sucelja
	XPs2_Recv(&Ps2Inst,&RxBuffer,1);

	//Podatak je primljen.
	if (IntrMask & XPS2_IPIXR_RX_FULL) {

		RxDataRecv = TRUE;
		RxNumBytes = ByteCount;

		printf("%x\r\n", RxBuffer);

		//3.Zadtak
		if(RxBuffer == 0x6B){
			ledsValue = (ledsValue << 1|ledsValue >> 7);
			XGpio_DiscreteWrite(&leds,1,ledsValue);
			flag = 1;
		} else if(RxBuffer == 0x74){
			ledsValue = (ledsValue >> 1 | ledsValue << 7);
			XGpio_DiscreteWrite(&leds,1,ledsValue);
			flag = 1;
		}

		if(RxBuffer == 0xF0){
			flag = 0;
		}

		/*if(flag && buffer == 0x6B ){
			ledsValue = (ledsValue << 1|ledsValue >> 7);
			XGpio_DiscreteWrite(&leds,1,ledsValue);
			flag = 0;
		} else if (flag && buffer == 0x74){
			ledsValue = (ledsValue >> 1 | ledsValue << 7);
			XGpio_DiscreteWrite(&leds,1,ledsValue);
			flag = 0;
		}*/


		/*if(flag){
			ledsValue = (ledsValue << 1|ledsValue >> 7);
			XGpio_DiscreteWrite(&leds,1,ledsValue);
			flag = 0;
		} else if (flag && buffer == 0x74){
			ledsValue = (ledsValue >> 1 | ledsValue << 7);
			XGpio_DiscreteWrite(&leds,1,ledsValue);
			flag = 0;
		*/
		buffer = RxBuffer;

		/*if(RxBuffer == 0x6B && buffer != 0xF0){
			ledsValue = (ledsValue << 1|ledsValue >> 7);
			XGpio_DiscreteWrite(&leds,1,ledsValue);
		} else if(RxBuffer == 0x74 && buffer !=0xF0){
			ledsValue = (ledsValue >> 1 | ledsValue << 7);
			XGpio_DiscreteWrite(&leds,1,ledsValue);
		}
		buffer = RxBuffer;
		*/



		//printf ("Interrupt mask: XPS2_IPIXR_RX_FULL\r\n");
		//2.Zadtak
		//counter++
		//XGpio_DiscreteWrite(&leds,1,counter);

		//Zadatak 1.
		/*if(RxBuffer == 0x16){
			XGpio_DiscreteWrite(&leds,1,0b00000001);
		} else if(RxBuffer == 0x1E){
			XGpio_DiscreteWrite(&leds,1,0b00000010);
		} else if(RxBuffer == 0x26){
			XGpio_DiscreteWrite(&leds,1,0b00000100);
		} else if(RxBuffer == 0x25){
			XGpio_DiscreteWrite(&leds,1,0b00001000);
		} else if(RxBuffer == 0x2E) {
			XGpio_DiscreteWrite(&leds,1,0b00010000);
		} else if(RxBuffer == 0x36){
			XGpio_DiscreteWrite(&leds,1,0b00100000);
		} else if(RxBuffer == 0x3D){
			XGpio_DiscreteWrite(&leds,1,0b01000000);
		} else if(RxBuffer == 0x3E){
			XGpio_DiscreteWrite(&leds,1,0b10000000);
		}*/


	}

	//Primljena je pogreška.
	if (IntrMask & XPS2_IPIXR_RX_ERR) {

		RxError = TRUE;
		//printf ("Interrupt mask: XPS2_IPIXR_RX_ERR\r\n");
	}

	//Primljen prelijev.
	if (IntrMask & XPS2_IPIXR_RX_OVF) {

		RxOverFlow = TRUE;
		//printf ("Interrupt mask: XPS2_IPIXR_RX_OVF\r\n");
	}

	//Transmission of the specified data is completed.
	if (IntrMask & XPS2_IPIXR_TX_ACK) {

		TxDataSent = TRUE;
		TxNumBytes = ByteCount;
		//printf ("Interrupt mask: XPS2_IPIXR_TX_ACK\r\n");
	}

	//Nije poslan ACK.
	if (IntrMask & XPS2_IPIXR_TX_NOACK) {

		TxNoAck = TRUE;
		//printf ("Interrupt mask: XPS2_IPIXR_TX_NOACK\r\n");
	}

	//Timeout slanaja.
	if (IntrMask & XPS2_IPIXR_WDT_TOUT) {

		TimeOut = TRUE;
		//printf ("Interrupt mask: XPS2_IPIXR_WDT_TOUT\r\n");
	}
}

/****************************************************************************/
/**
* Funkcija za postaljanje sustava prekida tako da se oni mogu dogoditi za
* XPS PS2 kontroler.
*
* @param	IntcInstPtr je pokazivač na varijablu tipa XIntc.
* @param	Ps2Ptr je pokazivač na varijablu tipa XPs2 koja će biti povezana
*			s upravljačem prekida.
* @param	IntrId je konstanta XPAR_<INTC_instance>_<PS2_instance>_VEC_ID iz
*			datoteke xparameters.h.
*
* @return	XST_SUCCESS ako je uspješno završena ili XST_FAILURE u suprotnom.
*
* @note		None.
*
****************************************************************************/
static int Ps2SetupIntrSystem(XIntc* IntcInstPtr, XPs2 *Ps2Ptr, u8 IntrId )
{
	int Status;

	// Inicijalizirati upravljač prekidima
	Status = XIntc_Initialize(IntcInstPtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Povezivanje kontrolera s prekidnom rutinom
	Status = XIntc_Connect(IntcInstPtr,
		 		IntrId,
				(XInterruptHandler) XPs2_IntrHandler,
				Ps2Ptr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Postaviti mod rada upravljača prekida
	Status = XIntc_Start(IntcInstPtr,XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Omogućiti rad upravljača prekidima
	XIntc_Enable(IntcInstPtr,INTR_ID);

	//Inicijalizacija iznimaka
	Xil_ExceptionInit();

	//Povezivanje prekidne rutine s iznimkama
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XIntc_InterruptHandler,
				IntcInstPtr);

	//Omogućavanje iznimki
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}