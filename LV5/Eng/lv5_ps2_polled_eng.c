#include "xps2.h"
#include "xparameters.h"
#include "xstatus.h"
#include "stdio.h"

//***********************************************TO DO 9*************************************************//
//**************************************Define constant KEYBOARD_ACK*************************************//


//******************************************************************************************************//


/**************************Function prototypes ******************************/
int Ps2Initialize(u16 Ps2DeviceId);
int Ps2SendData(u8 data);
int Ps2ReceiveData();


/*********************** Global variables definition ************************/
static XPs2 Ps2Inst;
u8 SendCODE = 0xED;
u8 RxBuffer;


int main(void)
{

	int Status, i, j;

	Status = Ps2Initialize(XPAR_PS2_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	print("Sending data to keyboard\r\n");
	for(i=0; i<8; i++){

		//***********************************************TO DO 6************************************************//
		//***********Send value of variable SendCODE and value of variable i using PS2 interface****************//

		
		
		//******************************************************************************************************//

		//delay od cca 1s
		for(j=0; j<66670000/12; j++);
	}

	print("Receiving data from keyboard:\r\n");
	while(1){

		//***********************************************TO DO 7*************************************************//
		//**********************************Receive 1 byte data from the keyboard********************************//

		
		//******************************************************************************************************//


		if (Status != XST_SUCCESS){
			return XST_FAILURE;
			print("Data receiving FAILED!\r\n");
		}
		else
			//***********************************************TO DO 8********************************************//
			//******************Print value received from the keyboard in hexadecimal format********************//

			
			//**************************************************************************************************//
	}
}

/****************************************************************************/
/* Function for initialization of keyboard which receives keyboard device ID
****************************************************************************/
int Ps2Initialize(u16 Ps2DeviceId)
{
	XPs2_Config *Ps2ConfigPtr;

	//***********************************************TO DO 1*************************************************//
	//************Grab PS2 interface configuration, save return value in variable Ps2ConfigPtr***************//

	
	//******************************************************************************************************//
	if (Ps2ConfigPtr == NULL) {
		print("Config Lookup FAILED!\r\n");
		return XST_FAILURE;
	}
	else
		print("Config Lookup SUCCESS!\r\n");


	//***********************************************TO DO 2*************************************************//
	//***************************************Initialize PS2 controller***************************************//

	
	//*******************************************************************************************************//

	print("PS2 Initialize SUCCESS!\r\n");
	return XST_SUCCESS;
}

/****************************************************************************/
/* Function for sending data using PS2 interface.
 * Function receives data which should be sent.
****************************************************************************/
int Ps2SendData(u8 data){

	u32 StatusReg;
	u32 BytesSent;
	u32 BytesReceived;
	int ACK_WAIT = 10000;
	
	//***********************************************TO DO 3*************************************************//
	//************Send 1 byte data using PS2 interface, save return value in variable BytesSent**************//

	
	//*******************************************************************************************************//
	if( BytesSent != 1) {
		print("Data Send FAILED!\r\n");
		return XST_FAILURE;
	}

	//Wait until keyboard sends response (ACK byte)
	do {
		StatusReg = XPs2_GetStatus(&Ps2Inst);
		ACK_WAIT--;
	}while((StatusReg & XPS2_STATUS_RX_FULL) == 0 && ACK_WAIT > 0);

	//***********************************************TO DO 4*************************************************//
	//********Receive 1 byte data using PS2 interface, save return value in variable BytesReceived***********//

	
	//*******************************************************************************************************//

	//Check if ACK byte is received
	if (RxBuffer != KEYBOARD_ACK) {
		print("ACK byte is not received!\r\n");
		return XST_FAILURE;
	}
	else
		xil_printf("Data %x Send SUCESS!\r\n", data);

	return XST_SUCCESS;
}

/****************************************************************************/
/* Function for receiving data using PS2 interface.
****************************************************************************/
int Ps2ReceiveData(){

	u32 StatusReg;
	u32 BytesReceived;

	do {
		StatusReg = XPs2_GetStatus(&Ps2Inst);
	}while((StatusReg & XPS2_STATUS_RX_FULL) == 0);

	//***********************************************TO DO 5*************************************************//
	//********Receive 1 byte data using PS2 interface, save return value in variable BytesReceived***********//

	
	//*******************************************************************************************************//

	return XST_SUCCESS;
}
