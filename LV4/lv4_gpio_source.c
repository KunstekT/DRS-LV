/***************************** Include Files *********************************/

#include <stdio.h>
#include "xintc.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xil_exception.h"

/************************** Definicije konstanti *****************************/
#define GPIO_DEVICE_ID				XPAR_PUSH_BUTTONS_4BITS_DEVICE_ID
#define INTC_DEVICE_ID				XPAR_INTC_0_DEVICE_ID

#define INTC_GPIO_INTERRUPT_ID		XPAR_INTC_0_GPIO_2_VEC_ID
#define GPIO_CHANEL_INTERRUPT_MASK	1

#define INTR_DELAY					0x000FFFFF
/************************** Prototipi funkcija ******************************/
void GpioHandler(void *CallBackRef);

int GpioIntrInit(XIntc *IntcInstancePtr, XGpio *InstancePtr,
			u16 DeviceId, u16 IntcDeviceId, u16 IntrId, u16 IntrMask);


//***********************************************TO DO 1*************************************************//
//**********************************Deklarirati varijable Gpio i Intc************************************//
XGpio gpio;
XIntc Intc;
//******************************************************************************************************//


//deklaracija globalne varijable koja je potrebna u prekidnoj rutini
static u16 GlobalIntrMask;

//***********************************************TO DO 2************************************************//
//******************************Deklarirati globalnu varijablu IntrCnt ****************************//
volatile int IntrCnt;

//******************************************************************************************************//

int main(void)
{
	int Status;

	print(" Press button to Generate Interrupt\r\n");

	Status = XGpio_Initialize(&Gpio, GPIO_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = GpioIntrInit(&Intc, &Gpio, GPIO_DEVICE_ID, INTC_DEVICE_ID, INTC_GPIO_INTERRUPT_ID, GPIO_CHANEL_INTERRUPT_MASK);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}



/******************************************************************************/
/**
*
* Inicijalizacija GPIO kontrolera i upravljača prekidima.
* Funkcija prima sljedeće parametre:
*
* @param	IntcInstancePtr - pokazivač na varijablu tipa XIntc,
* @param	InstancePtr - pokazivač na varijablu tipa XGpio,
* @param	DeviceId - vrijednost konstante XPAR_<GPIO_instance>_DEVICE_ID iz datoteke xparameters.h,
* @param	IntrId - vrijednost konstante XPAR_<INTC_instance>_<GPIO_instance>_IP2INTC_IRPT_INTR iz datoteke xparameters.h,
* @param	IntcDeviceId - vrijednost konstante XPAR_<Intc_instance>_DEVICE_ID iz datoteke xparameters.h,
* @param	IntrMask - maska GPIO kanala (broj kanala)
*
* @return	XST_SUCCESS ako je inicijalizacija uspješna, a u suprotno funkcija vraća XST_FAILURE
*
******************************************************************************/
int GpioIntrInit(XIntc *IntcInstancePtr, XGpio *InstancePtr,
			u16 DeviceId, u16 IntcDeviceId, u16 IntrId, u16 IntrMask)
{
	int Status;

	GlobalIntrMask = IntrMask;

	//**********************************************TO DO 3************************************************//
	//****Inicijalizirati upravljač prekidima, povratnu vrijednost funkcije spremiti u varijablu Status****//
	Status = XIntc_Initialize(IntcInstancePtr, IntrId);
	//*****************************************************************************************************//
	if (Status != XST_SUCCESS) {
		return Status;
	}

	//Povezivanje prekidne rutine
	XIntc_Connect(IntcInstancePtr, IntrId,
		      (Xil_ExceptionHandler)GpioHandler, InstancePtr);


	//**********************************************TO DO 4************************************************//
	//*********************************Omogućiti rad upravljača prekidima**********************************//
	void XIntc_Enable(InstancePtr, IntrId);
	//*****************************************************************************************************//

	//***********************************************TO DO 5***********************************************//
	//***Postaviti mod rada upravljača prekida, povratnu vrijednost funkcije spremiti u varijablu Status***//
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	//*****************************************************************************************************//
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the GPIO channel interrupts so that push button can be
	 * detected and enable interrupts for the GPIO device
	 */
	//**********************************************TO DO 6************************************************//
	//**************************Omogućiti generiranje prekida s GPIO kontrolerom***************************//
	XGpio_InterruptEnable(InstancePtr, IntrMask);
	XGpio_InterruptGlobalEnable(InstancePtr);
	//*****************************************************************************************************//

	//Inicijalizacija tablice iznimki
	Xil_ExceptionInit();

	//Registriranje upravljača prekidima i tablice iznimki
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)XIntc_InterruptHandler, IntcInstancePtr);

	//Omogućavanje nekritičnih iznimki
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

//Prekidna rutina
void GpioHandler(void *CallbackRef)
{
	XGpio *GpioPtr = (XGpio *)CallbackRef;
	u32 delay = 0;

	//**********************************************TO DO 7************************************************//
	//******************************************Onemogućiti prekide****************************************//
	XGpio_InterruptDisable(GpioPtr, GPIO_CHANEL_INTERRUPT_MASK);
	XIntc_Disable(&Intc, INTC_GPIO_INTERRUPT_ID) //??
	//*****************************************************************************************************//

	int pushValue;

	//**********************************************TO DO 8************************************************//
	//***************************************Pročitati stanje tipkala**************************************//
	pushValue = XGpio_DiscreteRead(GpioPtr, 1);
	//*****************************************************************************************************//

	//**********************************************TO DO 9************************************************//
	//*********************************Inkrementirati varijablu IntrCnt************************************//
	IntrCnt++;
	//*****************************************************************************************************//

	xil_printf("Interrupt Handler no. %d - button value: %d!\r\n", IntrCnt, pushValue);

	while(delay < INTR_DELAY)
		delay++;

	//**********************************************TO DO 10***********************************************//
	//******************************************Omogućiti prekide******************************************//
	XGpio_InterruptEnable(GpioPtr, GPIO_CHANEL_INTERRUPT_MASK);
	XIntc_Enable(&Intc, INTC_GPIO_INTERRUPT_ID);	
	//*****************************************************************************************************//

	//**********************************************TO DO 11***********************************************//
	//*************************************Obrisati prekidne zahtjeve**************************************//
	XGpio_InterruptClear(GpioPtr, GPIO_CHANEL_INTERRUPT_MASK);
	//*****************************************************************************************************//
}

