#include "stdio.h"
#include "xil_exception.h"
#include "xstatus.h"
//***********************************************TO DO 1*************************************************//
//*************************Uključiti datoteke xparameters.h, xps2.h, xintc.h ****************************//




//******************************************************************************************************//

//***********************************************TO DO 2*************************************************//
//*********************Definirati konstante PS2_DEVICE_ID, INTC_DEVICE_ID i INTR_ID**********************//




//******************************************************************************************************//

/*************************** Definiranje makroa *****************************/
#define printf xil_printf

/************************** Prototipi funkcija ******************************/

int Ps2IntrExample(XIntc* IntcInstPtr, XPs2* Ps2InstPtr, u16 Ps2DeviceId, u8 Ps2IntrId);

static void Ps2IntrHandler(void *CallBackRef, u32 Event, u32 EventData);

static int Ps2SetupIntrSystem(XIntc* IntcInstPtr, XPs2 *Ps2Ptr, u8 IntrId);

//***********************************************TO DO 3*************************************************//
//*********************************Deklarirati varijable Ps2Inst i IntcInst******************************//


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

//***********************************************TO DO 4*************************************************//
//************************************Deklarirati varijablu RxBuffer*************************************//

//*******************************************************************************************************//

int main(void)
{
	int Status;

//***********************************************TO DO 5*************************************************//
//******Pozvati funkciju Ps2IntrExample, povratnu vrijednost funkcije spremiti u varijablu Status********//

	
//*******************************************************************************************************//

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

	//***********************************************TO DO 6*************************************************//
	//******Dohvatiti konfiguraciju PS2 sučelja, povratnu vrijednost spremiti u varijablu ConfigPtr**********//

	
	//*******************************************************************************************************//
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}

	//***********************************************TO DO 7*************************************************//
	//*************************************Inicijalizirati PS2 kontroler*************************************//

	
	//*******************************************************************************************************//

	//Self Test PS/2 uređaja
	Status = XPs2_SelfTest(Ps2InstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//***********************************************TO DO 8*************************************************//
	//*******Pozvati funkciju Ps2SetupIntrSystem, povratnu vrijednost spremiti u varijablu Status************//

	
	//*******************************************************************************************************//
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Postavljanje prekidne rutine
	XPs2_SetHandler(&Ps2Inst, (XPs2_Handler)Ps2IntrHandler, &Ps2Inst);

	//***********************************************TO DO 9*************************************************//
	//******Omogućiti prekide za prijem podataka i globalno omogućiti prekide u XPS PS2 kontroleru***********//

	
	
	//*******************************************************************************************************//

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
	//***********************************************TO DO 10*************************************************//
	//********************************Primiti 1 byte podataka putem PS2 sucelja*******************************//

	
	//********************************************************************************************************//

	//Podatak je primljen.
	if (IntrMask & XPS2_IPIXR_RX_FULL) {

		RxDataRecv = TRUE;
		RxNumBytes = ByteCount;

		printf("%x\r\n", RxBuffer);
		//printf ("Interrupt mask: XPS2_IPIXR_RX_FULL\r\n");
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


	//**********************************************TO DO 11*************************************************//
	//****Inicijalizirati upravljač prekidima, povratnu vrijednost funkcije spremiti u varijablu Status******//

	
	//*******************************************************************************************************//
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

	//***********************************************TO DO 12*****************************+****************//
	//***Postaviti mod rada upravljača prekida, povratnu vrijednost funkcije spremiti u varijablu Status***//

	
	//*****************************************************************************************************//
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//**********************************************TO DO 13******************************+****************//
	//*********************************Omogućiti rad upravljača prekidima**********************************//

	
	//*****************************************************************************************************//

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
