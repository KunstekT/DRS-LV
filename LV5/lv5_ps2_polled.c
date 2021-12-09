#include "xps2.h"
#include "xparameters.h"
#include "xstatus.h"
#include "stdio.h"

//***********************************************TO DO 9*************************************************//
//***********************************Definirati konstantu KEYBOARD_ACK***********************************//
const int KEYBOARD_ACK = /*TODO*/ ; // ili #define KEYBOARD_ACK = ?

//******************************************************************************************************//


/************************** Prototipi funkcija ******************************/
int Ps2Initialize(u16 Ps2DeviceId);
int Ps2SendData(u8 data);
int Ps2ReceiveData();


/******************** Definicija globalnih varijabli ***********************/
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
		//***Poslati vrijednost varijable SendCODE i nakon toga vrijednost varijable i putem PS2 sučelja********//
		Ps2SendData(SendCODE);
		Ps2SendData(i);
		
		//******************************************************************************************************//

		//delay od cca 1s
		for(j=0; j<66670000/12; j++);
	}

	print("Receiving data from keyboard:\r\n");
	while(1){

		//***********************************************TO DO 7*************************************************//
		//*********************************Primiti 1 byte podataka od tipkovnice********************************//
		Status = Ps2ReceiveData();
		
		//******************************************************************************************************//


		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
			print("Data receiving FAILED!\r\n");
		}
		else
			//***********************************************TO DO 8********************************************//
			//****************Ispisati vrijednost primljenu od tipkovnice u heksadecimalnom obliku**************//
			Xil_printf("%x", &Status);
			
			//**************************************************************************************************//
	}
}

/****************************************************************************/
/* Funkcija za inicijalizaciju tipkovnice koja prima device ID tipkovnice
****************************************************************************/
int Ps2Initialize(u16 Ps2DeviceId)
{
	XPs2_Config *Ps2ConfigPtr;

	//***********************************************TO DO 1*************************************************//
	//******Dohvatiti konfiguraciju PS2 sučelja, povratnu vrijednost spremiti u varijablu Ps2ConfigPtr*******//
	Ps2ConfigPtr = XPs2_LookupConfig(Ps2DeviceId);
	
	//******************************************************************************************************//
	if (Ps2ConfigPtr == NULL) {
		print("Config Lookup FAILED!\r\n");
		return XST_FAILURE;
	}
	else
		print("Config Lookup SUCCESS!\r\n");


	//***********************************************TO DO 2*************************************************//
	//*************************************Inicijalizirati PS2 kontroler*************************************//
	XPs2_CfgInitialize(&Ps2Inst, Ps2ConfigPtr, Ps2ConfigPtr->BaseAddress); /* (XPs2 * InstancePtr, XPs2_Config * Config, u32 EffectiveAddr) */
	
	//*******************************************************************************************************//

	print("PS2 Initialize SUCCESS!\r\n");
	return XST_SUCCESS;
}

/****************************************************************************/
/* Funkcija za slanje podataka putem PS2 sučelja.
 * Funkcija prima podatak koji treba poslati.
****************************************************************************/
int Ps2SendData(u8 data){

	u32 StatusReg;
	u32 BytesSent;
	u32 BytesReceived;
	int ACK_WAIT = 10000;
	
	//***********************************************TO DO 3*************************************************//
	//****Poslati 1 byte podataka putem PS2 sucelja, povratnu vrijdnost spremiti u varijablu BytesSent*******//
	XPs2_Send(&Ps2Inst, data, (u32)1); /*(XPs2 * InstancePtr, u8 * BufferPtr, u32 NumBytes)*/
	
	//*******************************************************************************************************//
	if( BytesSent != 1) {
		print("Data Send FAILED!\r\n");
		return XST_FAILURE;
	}

	//Cekati dok tipkovnica ne posalje odgovor (ACK byte)
	do {
		StatusReg = XPs2_GetStatus(&Ps2Inst);
		ACK_WAIT--;
	}while((StatusReg & XPS2_STATUS_RX_FULL) == 0 && ACK_WAIT > 0);

	//***********************************************TO DO 4*************************************************//
	//****Primiti 1 byte podataka putem PS2 sucelja, povratnu vrijdnost spremiti u varijablu BytesReceived***//
	BytesReceived = XPs2_Recv(&Ps2Inst, RxBuffer, (u32)1); /*(XPs2 * InstancePtr, u8 * BufferPtr, u32 NumBytes)*/
	
	//*******************************************************************************************************//

	//Provjeriti jel primljen ACK byte
	if (RxBuffer != KEYBOARD_ACK) {
		print("Nije primljen ACK byte!\r\n");
		return XST_FAILURE;
	}
	else
		xil_printf("Data %x Send SUCESS!\r\n", data);

	return XST_SUCCESS;
}

/****************************************************************************/
/* Funkcija za primanje podataka putem PS2 sučelja.
****************************************************************************/
int Ps2ReceiveData(){

	u32 StatusReg;
	u32 BytesReceived;

	do {
		StatusReg = XPs2_GetStatus(&Ps2Inst);
	}while((StatusReg & XPS2_STATUS_RX_FULL) == 0);

	//***********************************************TO DO 5*************************************************//
	//***Primiti 1 byte podataka putem PS2 sucelja, povratnu vrijdnost spremiti u varijablu BytesReceived****//
	BytesReceived = XPs2_Recv(&Ps2Inst, RxBuffer, (u32)1); /*(XPs2 * InstancePtr, u8 * BufferPtr, u32 NumBytes)*/
	
	//*******************************************************************************************************//

	return XST_SUCCESS;
}
