#include <LPC210X.H>
#include "uart.h"
#include "string.h"
#include "FreeRTOS.h"
#include "queue.h"

/************ UART ************/
//pin control register
#define mP0_1_RX0_PIN_MODE 							0x00000004
#define mP0_0_TX0_PIN_MODE 							0x00000001

// U0LCR Line Control Register
#define mDIVISOR_LATCH_ACCES_BIT                   0x00000080
#define m8BIT_UART_WORD_LENGTH                     0x00000003

// UxIER Interrupt Enable Register
#define mRX_DATA_AVALIABLE_INTERRUPT_ENABLE        0x00000001
#define mTHRE_INTERRUPT_ENABLE                     0x00000002

// UxIIR Pending Interrupt Identification Register
#define mINTERRUPT_PENDING_IDETIFICATION_BITFIELD  0x0000000F
#define mTHRE_INTERRUPT_PENDING                    0x00000002
#define mRX_DATA_AVALIABLE_INTERRUPT_PENDING       0x00000004

/************ Interrupts **********/
// VIC (Vector Interrupt Controller) channels
#define VIC_UART0_CHANNEL_NR  6
#define VIC_UART1_CHANNEL_NR  7

// VICVectCntlx Vector Control Registers
#define mIRQ_SLOT_ENABLE                           0x00000020

xQueueHandle UartRxQueue, UartTxQueue;

void Uart_GetString( char* pcString ) {
	
	char cCharBuffer = 0;
	char cStringBuffer[UART_RX_BUFFER_SIZE];
	unsigned char ucStringIndex = 0;
	
	xQueueReceive(UartRxQueue, &cCharBuffer, portMAX_DELAY);
	
	while((cCharBuffer != TERMINATOR) && (ucStringIndex < UART_RX_BUFFER_SIZE - 1)){
		cStringBuffer[ucStringIndex] = cCharBuffer;
		ucStringIndex++;
	}
	
	cStringBuffer[ucStringIndex] = NULL;
	CopyString(cStringBuffer, pcString);
}
	
void Uart_PutString(char* pcString){
	
	unsigned char ucStringIndex = 0;
	unsigned char ucTerminator = TERMINATOR;
	char cFirstChar = NULL;
	
	while(( pcString[ucStringIndex] != NULL ) && ( ucStringIndex < UART_TX_BUFFER_SIZE - 1 )){
		xQueueSend(UartTxQueue, &pcString[ucStringIndex], portMAX_DELAY);
		ucStringIndex++;
	}
	xQueueSend(UartTxQueue, &ucTerminator, portMAX_DELAY);
	xQueueReceive(UartTxQueue, &cFirstChar, portMAX_DELAY);
	U0THR = cFirstChar;
}

///////////////////////////////////////////
__irq void UART0_Interrupt (void) {
   // jesli przerwanie z odbiornika (Rx)
   
   unsigned int uiCopyOfU0IIR=U0IIR; // odczyt U0IIR powoduje jego kasowanie wiec lepiej pracowac na kopii

   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING) // odebrano znak
   {
		 BaseType_t xHigherPriorityTaskWoken = pdTRUE;
     unsigned char ucBuffer = U0RBR;
		 xQueueSendFromISR(UartRxQueue, &ucBuffer, &xHigherPriorityTaskWoken);
   } 
   
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING)              // wyslano znak - nadajnik pusty 
   {
			BaseType_t xHigherPriorityTaskWoken = pdTRUE;
			unsigned char ucBuffer = 0;
			xQueueReceiveFromISR(UartTxQueue, &ucBuffer, &xHigherPriorityTaskWoken);
			U0THR = ucBuffer;
			// narazie nic nie wysyłamy
   }

   VICVectAddr = 0; // Acknowledge Interrupt
}

////////////////////////////////////////////
void UART_InitWithInt(unsigned int uiBaudRate){
	
	unsigned long ulDivisor, ulWantedClock;
	ulWantedClock=uiBaudRate*16;
	ulDivisor=15000000/ulWantedClock;
	
	// UART
	PINSEL0 = PINSEL0 | 0x55;                                     // ustawic piny uar0 odbiornik nadajnik
	U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT; // d³ugosc s³owa, DLAB = 1
	U0DLL = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	ulDivisor >>= 8;
	U0DLM = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       // DLAB = 0
	U0IER  |= mRX_DATA_AVALIABLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE ;               

	// INT
	VICVectAddr1  = (unsigned long) UART0_Interrupt;             // set interrupt service routine address
	VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     // use it for UART 0 Interrupt
	VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);               // Enable UART 0 Interrupt Channel

	UartRxQueue = xQueueCreate(UART_RX_BUFFER_SIZE, sizeof(char));
	UartTxQueue = xQueueCreate(UART_TX_BUFFER_SIZE, sizeof(char));
}

