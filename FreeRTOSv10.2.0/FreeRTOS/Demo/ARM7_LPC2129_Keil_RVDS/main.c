#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "uart.h"

void UartRx( void *pvParameters ){

		while(1){
			Led_Toggle(cUart_GetChar() - '0');
		}
}

int main( void ){

		Led_Init();
		UART_InitWithInt(9600);
		xTaskCreate( UartRx, NULL , 100 , NULL, 1 , NULL );
		vTaskStartScheduler();
	
	while(1);
}
