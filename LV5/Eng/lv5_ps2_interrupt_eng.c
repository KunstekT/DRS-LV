#include "stdio.h"
#include "xil_exception.h"
#include "xstatus.h"
//***********************************************TO DO 1*************************************************//
//************************Include header files xparameters.h, xps2.h, xintc.h ***************************//




//******************************************************************************************************//

//***********************************************TO DO 2*************************************************//
//**********************Define constants PS2_DEVICE_ID, INTC_DEVICE_ID i INTR_ID*************************//




//******************************************************************************************************//

/****************************** Define makros *******************************/
#define printf xil_printf

/************************** Function prototypes *****************************/

int Ps2IntrExample(XIntc* IntcInstPtr, XPs2* Ps2InstPtr, u16 Ps2DeviceId, u8 Ps2IntrId);

static void Ps2IntrHandler(void *CallBackRef, u32 Event, u32 EventData);

static int Ps2SetupIntrSystem(XIntc* IntcInstPtr, XPs2 *Ps2Ptr, u8 IntrId);

//***********************************************TO DO 3*************************************************//
//**********************************Declare variables Ps2Inst and IntcInst*******************************//


//******************************************************************************************************//

//Variables which are used in interrupt handler
volatile static int RxDataRecv = FALSE; 	/* Flag to indicate Receive Data */
volatile static int RxError = FALSE; 		/* Flag to indicate Receive Error */
volatile static int RxOverFlow = FALSE; 	/* Flag to indicate Receive Overflow */
volatile static int TxDataSent = FALSE; 	/* Flag to indicate Tx Data sent */
volatile static int TxNoAck = FALSE;    	/* Flag to indicate Tx No Ack */
volatile static int TimeOut = FALSE;   		/* Flag to indicate Watchdog Timeout */
volatile static int TxNumBytes = 0;			/* Number of bytes transmitted */
volatile static int RxNumBytes = 0;     	/* Number of bytes received */

//***********************************************TO DO 4*************************************************//
//**************************************Declare variable RxBuffer****************************************//

//*******************************************************************************************************//

int main(void)
{
	int Status;

//***********************************************TO DO 5*************************************************//
//*****************Call function Ps2IntrExample, save return value in variable Status********************//

	
//*******************************************************************************************************//

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;

}

/****************************************************************************/
/**
*
* This function is example of initialization and communication with the keyboard in interrupt mode.
*
*
* @param	IntcInstPtr is pointer on variable of type XIntc.
* @param	Ps2InstPtr is pointer on variable of type XPs2.
* @param	Ps2DeviceId is device id of XPS PS2 controller from the file xparameters.h.
* @param	Ps2IntrId is value of constant XPAR_<INTC_instance>_<PS2_instance>_VEC_ID from the file xparameters.h.
* @return
*		- XST_SUCCESS if function is executed successfully.
*		- XST_FAILURE if function is not executed successffully.
*
* @note		Function will never return XST_SUCCESS if infinite loop break is not implemented.
****************************************************************************/
int Ps2IntrExample(XIntc* IntcInstPtr, XPs2* Ps2InstPtr,
			u16 Ps2DeviceId, u8 Ps2IntrId)
{
	int Status;
	XPs2_Config *ConfigPtr;

	//***********************************************TO DO 6*************************************************//
	//**************Grab PS2 interface configuration, save return valuein variable ConfigPtr*****************//

	
	//*******************************************************************************************************//
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}

	//***********************************************TO DO 7*************************************************//
	//***************************************Initialize PS2 controller***************************************//

	
	//*******************************************************************************************************//

	//Self Test PS/2 uređaja
	Status = XPs2_SelfTest(Ps2InstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//***********************************************TO DO 8*************************************************//
	//****************Call function Ps2SetupIntrSystem, save return value in variable Status*****************//

	
	//*******************************************************************************************************//
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Setting uo interrupt routine
	XPs2_SetHandler(&Ps2Inst, (XPs2_Handler)Ps2IntrHandler, &Ps2Inst);

	//***********************************************TO DO 9*************************************************//
	//*****Enable interrupts for data receive and globally enable interrupts in XPS PS2 controller***********//

	
	
	//*******************************************************************************************************//

	printf("\r\n Press Keys on the keyboard \r\n");

	//Infinite loop
	while(1)
	{

	}

	//Disabling receive interrupts and globally disabling interrupts in XPS PS2 controller
	//XPs2_IntrDisable(&Ps2Inst, XPS2_IPIXR_ALL);
	//XPs2_IntrGlobalDisable(&Ps2Inst);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
* Interrupt routine which is called from processor in the cases when XPS PS2
* controller generates one of the following interrupts:
*	- Data receiving		(Receive Data Interrupt)
*	- Error receive		 	(Receive Error Interrupt)
*	- Overflow receive		(Receive Overflow Interrupt)
*	- Data sending			(Transmit Data Interrupt)
*	- ACK not received		(Transmit No ACK Interrupt)
*	- Watchdog timeout		(Watchdog Timeout Interrupt)
*
* @param 	CallBackRef is callback reference which is passed to interrupt routine.
* @param	IntrMask is mask which tells which event has caused interrupt.
*			In the file xps2_l.h are located constants XPS2_IPIXR_* which
*			describe events which caused interrupt:
*		- XPS2_IPIXR_RX_FULL for Data receive
*		- XPS2_IPIXR_RX_ERR for Error receive
*		- XPS2_IPIXR_RX_OVF for Overflow receive
*		- XPS2_IPIXR_TX_ACK for Data sending
*		- XPS2_IPIXR_TX_NOACK for ACK not received
*		- XPS2_IPIXR_WDT_TOUT for Watchdog timeout
* @param 	ByteCount contains number of received/sent bytes at the time of call.
*
* @return	None.
*
* @note		None.*
******************************************************************************/
static void Ps2IntrHandler(void *CallBackRef, u32 IntrMask, u32 ByteCount)
{
	//***********************************************TO DO 10*************************************************//
	//*********************************Receive 1 byte data using PS2 interface********************************//

	
	//********************************************************************************************************//

	//Data is received.
	if (IntrMask & XPS2_IPIXR_RX_FULL) {

		RxDataRecv = TRUE;
		RxNumBytes = ByteCount;

		printf("%x\r\n", RxBuffer);
		//printf ("Interrupt mask: XPS2_IPIXR_RX_FULL\r\n");
	}

	//Error is received.
	if (IntrMask & XPS2_IPIXR_RX_ERR) {

		RxError = TRUE;
		//printf ("Interrupt mask: XPS2_IPIXR_RX_ERR\r\n");
	}

	//Overflow is received.
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

	//ACK is not sent.
	if (IntrMask & XPS2_IPIXR_TX_NOACK) {

		TxNoAck = TRUE;
		//printf ("Interrupt mask: XPS2_IPIXR_TX_NOACK\r\n");
	}

	//Sending timeout.
	if (IntrMask & XPS2_IPIXR_WDT_TOUT) {

		TimeOut = TRUE;
		//printf ("Interrupt mask: XPS2_IPIXR_WDT_TOUT\r\n");
	}
}

/****************************************************************************/
/**
* Function for configuring interrupts for XPS PS2 controller.
*
* @param	IntcInstPtr is pointer on variable of type XIntc.
* @param	Ps2Ptr is pointer on variable of type XPs2 which will be connected
*			with interrupt controller.
* @param	IntrId is constant XPAR_<INTC_instance>_<PS2_instance>_VEC_ID from
*			the file xparameters.h.
*
* @return	XST_SUCCESS if it is successffully executed or XST_FAILURE otherwise.
*
* @note		None.
*
****************************************************************************/
static int Ps2SetupIntrSystem(XIntc* IntcInstPtr, XPs2 *Ps2Ptr, u8 IntrId )
{
	int Status;


	//**********************************************TO DO 11*************************************************//
	//***************Initialize interrupt controller, save return value in variable Status*******************//

	
	//*******************************************************************************************************//
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Connectiong controller with interrupt routine
	Status = XIntc_Connect(IntcInstPtr,
		 		IntrId,
				(XInterruptHandler) XPs2_IntrHandler,
				Ps2Ptr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//***********************************************TO DO 12*****************************+****************//
	//***************Set interrupt controller mode, save return value in variable Status*******************//

	
	//*****************************************************************************************************//
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//**********************************************TO DO 13******************************+****************//
	//*************************************Enable interrupt controller*************************************//

	
	//*****************************************************************************************************//

	//Exception initialization
	Xil_ExceptionInit();

	//Connectiong interrupt routine with exceptions
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XIntc_InterruptHandler,
				IntcInstPtr);

	//Enable exceptions
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
