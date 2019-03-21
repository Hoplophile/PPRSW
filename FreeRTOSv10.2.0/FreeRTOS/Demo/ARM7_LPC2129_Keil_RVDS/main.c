#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "string.h"
#include "uart.h"
#include "keyboard.h"

void Rtos_Transmiter_SendString (char cMessage[], void *pSemaphore){
	
	
	
	if(xSemaphoreTake(*((xSemaphoreHandle*)pSemaphore), portMAX_DELAY) == pdTRUE) {
			Transmiter_SendString(cMessage);
			while (Transmiter_GetStatus()!=FREE){};
			xSemaphoreGive(*((xSemaphoreHandle*)pSemaphore));
	}
}

void LettersTx (void *pSemaphore ){	
	
	unsigned int uiStartTick = 0, uiExeTime = 0;
	char cMessage[100];
	
	while(1){		
		CopyString("-ABCDEEFGH-:", cMessage);
		AppendUIntToString(uiExeTime, cMessage);
		AppendString("\n\r", cMessage);
		uiStartTick = xTaskGetTickCount();
		Rtos_Transmiter_SendString(cMessage, pSemaphore);
		uiExeTime = xTaskGetTickCount() - uiStartTick;
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pSemaphore ){
	
	while(1){
		if(eKeyboard_Read() != RELASED) Rtos_Transmiter_SendString("-Keyboard-\n", pSemaphore);
		vTaskDelay(100);
	}
}

int main( void ){
	
	xSemaphoreHandle xSemaphore;
	
	ButtonInit();
	UART_InitWithInt(300);
	vSemaphoreCreateBinary(xSemaphore);
	xTaskCreate(LettersTx, NULL, 128, &xSemaphore, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, &xSemaphore, 1, NULL );
	vTaskStartScheduler();
	
	while(1) {};
}
