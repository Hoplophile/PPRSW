#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "string.h"
#include "uart.h"
#include "keyboard.h"

void LettersTx (void *pvParameters){
	
	while(1){
		Transmiter_SendString("-ABCDEEFGH-\n");
		while (Transmiter_GetStatus()!=FREE){};
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
	
	while(1){
		if(eKeyboard_Read() != RELASED){
			Transmiter_SendString("-Keyboard-\n");
			while (Transmiter_GetStatus()!=FREE){};
		}
	}
}

int main( void ){
	
	ButtonInit();
	UART_InitWithInt(300);
	xTaskCreate(LettersTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 1, NULL );
	vTaskStartScheduler();
	
	while(1) {};
}
