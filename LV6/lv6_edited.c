#include "xtft.h"
#include "xparameters.h"

// TFT_FRAME_ADDR predstavlja Base Address Micron RAM memorije
#define TFT_FRAME_ADDR		0x85000000

//Color Values.
#define FGCOLOR_VALUE		0x00FFFF00	/**< Foreground Color - Yellow? */
#define BGCOLOR_VALUE		0x0		/**< Background Color - Black */
#define RED_COLOR 	0x00FF0000	/**< Color - White */
#define BLUE_COLOR	0x000000FF	/**< Color - White */
#define GREEN_COLOR	0x0000FF00	/**< Color - White */
#define WHITECOLOR_VALUE 	0x00FFFFFF	/**< Color - White */

// #define X1POS	120	/**< Column Start Position */
// #define X2POS	120	 /**< Column End Position */
// #define Y1POS	30 	/**< Row Start Position */
// #define Y2POS	430	 /**< Row End Position */

int TftInitialize(u32 TftDeviceId);
int TftWriteString(XTft *InstancePtr, const u8 *CharValue);

static XTft TftInstance;

int main() {

	int Status;
	u8 Znak;

	// inicijalizacija 'static XTft TftInstance'
	Status = TftInitialize(XPAR_VGA_CONTROLLER_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Očistiti ekran, postaviti boju pozadine i boju slova i ispisati nekoliko znakova na ekran (ispisuje se 'SSSR' u crvenoj boji)
	XTft_ClearScreen(&TftInstance);

	u8 VarChar = 'S';
	XTft_SetColor(&TftInstance, FGCOLOR_VALUE, RED_COLOR);

	XTft_Write(&TftInstance, VarChar);
	XTft_Write(&TftInstance, VarChar);
	XTft_Write(&TftInstance, VarChar);
	XTft_Write(&TftInstance, 'R');

	// Očistiti ekran, ispisati proizvoljan string na ekran
	XTft_SetColor(&TftInstance, FGCOLOR_VALUE, BGCOLOR_VALUE);
	XTft_ClearScreen(&TftInstance);
	XTft_SetColor(&TftInstance, FGCOLOR_VALUE, RED_COLOR);

	Status = TftWriteString(&TftInstance, (u8*) "TEKST\n");
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Očistiti ekran, nacrtati 4 vodoravne linije ekranu
	XTft_SetColor(&TftInstance, FGCOLOR_VALUE, BGCOLOR_VALUE);
	XTft_ClearScreen(&TftInstance);

	int index;
	for (index = 120; index <= 520; index++) {
		XTft_SetPixel(&TftInstance, index, 30, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, index, 50, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, index, 70, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, index, 90, FGCOLOR_VALUE);
	}

	//***********************************//
	// Očistiti ekran, iscrtati board za križić-kružić

	XTft_SetColor(&TftInstance, GREEN_COLOR, BGCOLOR_VALUE);
	XTft_ClearScreen(&TftInstance);

	for (index = 100; index <= 280; index++) {
		XTft_SetPixel(&TftInstance, index, 20, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, index, 40, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, index, 60, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, index, 80, FGCOLOR_VALUE);
	}
	
	for (index = 20; index <= 80; index++) {
		XTft_SetPixel(&TftInstance, 100, index, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, 160, index, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, 220, index, FGCOLOR_VALUE);
		XTft_SetPixel(&TftInstance, 280,index, FGCOLOR_VALUE);
	}

	return XST_SUCCESS;
}

int TftInitialize(u32 TftDeviceId) {

	int Status;
	XTft_Config *TftConfigPtr;

	// Dohvaćanje konfiguracije VGA sučelja
	TftConfigPtr = XTft_LookupConfig(TftDeviceId);
	if (TftConfigPtr == (XTft_Config *) NULL) {
		return XST_FAILURE;
	}

	// Inicijalizacija VGA kontrolera
	Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr, TftConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Čekanje da Vsync status bit bude postavljen u 1 kako bi bili sigurni da je prethodni frame prikazan
	while (XTft_GetVsyncStatus(&TftInstance) != XTFT_IESR_VADDRLATCH_STATUS_MASK);

	// Postavljanje base adrese Video Memorije
	XTft_SetFrameBaseAddr(&TftInstance, TFT_FRAME_ADDR);
	//XTft_ClearScreen(&TftInstance);

	return XST_SUCCESS;
}

//funkcija ispisuje znak po znak na ekran sve dok ne dođe do null znaka ili kraja stringa
int TftWriteString(XTft *InstancePtr, const u8 *CharValue) {

	while (*CharValue != 0) {
		XTft_Write(InstancePtr, *CharValue);
		CharValue++;
	}

	return XST_SUCCESS;
}