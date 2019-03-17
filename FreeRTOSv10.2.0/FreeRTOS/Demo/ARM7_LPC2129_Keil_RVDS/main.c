#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "string.h"
#include "uart.h"
#include "keyboard.h"

unsigned int uiStartTick, uiExeTime;
char cMessage[] = "-ABCDEEFGH-:", cTimeCount[8];

void Rtos_Transmiter_SendString (char cMessage[], void *pSemaphore){
	
	if(xSemaphoreTake(*((xSemaphoreHandle*)pSemaphore), portMAX_DELAY) == pdTRUE) {
			Transmiter_SendString(cMessage);
			while (Transmiter_GetStatus()!=FREE){};
			xSemaphoreGive(*((xSemaphoreHandle*)pSemaphore));
	}
}

void LettersTx (void *pSemaphore ){	
	
	while(1){		
		UIntToHexStr(uiExeTime, cTimeCount);
		AppendString("\n", cTimeCount);
		AppendString(cTimeCount, cMessage);
		uiStartTick = xTaskGetTickCount();
		Rtos_Transmiter_SendString(cMessage, pSemaphore);
		uiExeTime = xTaskGetTickCount() - uiStartTick;
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pSemaphore ){
	
	while(1){
		if(eKeyboard_Read() != RELASED) Rtos_Transmiter_SendString("-Keyboard-\n", pSemaphore);
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
