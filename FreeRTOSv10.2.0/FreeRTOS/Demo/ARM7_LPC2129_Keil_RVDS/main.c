#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "string.h"
#include "uart.h"

void LettersTx (void *pvParameters){
	
	while(1){
		Transmiter_SendString("-ABCDEEFGH-\n");
		while (Transmiter_GetStatus()!=FREE){};
		vTaskDelay(300);
	}
}
int main( void ){
	
	UART_InitWithInt(9600);
	xTaskCreate(LettersTx, NULL, 128, NULL, 1, NULL );
	vTaskStartScheduler();
	
	while(1) {};
}
