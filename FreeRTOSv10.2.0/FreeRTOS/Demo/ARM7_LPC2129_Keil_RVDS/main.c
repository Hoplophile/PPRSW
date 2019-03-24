#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "string.h"
#include "uart.h"
#include "keyboard.h"
#include "queue.h"

void Rtos_Transmiter_SendString ( void *pvParameters ){
	
		QueueHandle_t *xMessagesQueue = (QueueHandle_t*)pvParameters;
		char cMessageBuffer[30];
	
		while(1){
			xQueueReceive(*xMessagesQueue, cMessageBuffer, portMAX_DELAY);
			Transmiter_SendString(cMessageBuffer);
			while (Transmiter_GetStatus()!=FREE){};
		}
}

void LettersTx ( void *pvParameters ){	
	
	QueueHandle_t *xMessagesQueue = (QueueHandle_t*)pvParameters;
	unsigned int uiStartTick = 0, uiExeTime = 0;
	char cLettersMessage[30];
	BaseType_t pdResult = errQUEUE_FULL;
	
	while(1){		
		CopyString("-ABCDEEFGH-:", cLettersMessage);
		AppendUIntToString(uiExeTime, cLettersMessage);
		AppendString("\n", cLettersMessage);
		
		uiStartTick = xTaskGetTickCount();		
		pdResult = xQueueSend(*xMessagesQueue, cLettersMessage, 0);
		uiExeTime = xTaskGetTickCount() - uiStartTick;
		if(pdResult == errQUEUE_FULL) {
			Led_Toggle(1);
		}
		vTaskDelay(300);
	}
}

void KeyboardTx ( void *pvParameters ){
	
	QueueHandle_t *xMessagesQueue = (QueueHandle_t*)pvParameters;
	char cKeyboardMessage[30];
	CopyString("-Keyboard-\n", cKeyboardMessage);
	
	while(1){
		if(eKeyboard_Read() != RELASED) {
		xQueueSend(*xMessagesQueue, cKeyboardMessage, portMAX_DELAY);
		vTaskDelay(100);
		}
	}
}

int main( void ){
	
	QueueHandle_t xMessagesQueue;
	
	Led_Init();
	ButtonInit();
	UART_InitWithInt(300);
	
	xMessagesQueue	= xQueueCreate(5, sizeof(char[30]));
	xTaskCreate(LettersTx, NULL, 128, &xMessagesQueue, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, &xMessagesQueue, 1, NULL );
	xTaskCreate(Rtos_Transmiter_SendString, NULL, 128, &xMessagesQueue, 1, NULL );
	vTaskStartScheduler();
	
	while(1) {};
}
