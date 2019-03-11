 #include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"

void Pulse_LED0( void *pSemaphore ){

	while(1){
		if(xSemaphoreTake(*((xSemaphoreHandle*)pSemaphore), portMAX_DELAY) == pdTRUE) {
			Led_Toggle(0);
			vTaskDelay(100);
			Led_Toggle(0);
		}
	}
}

void PulseTrigger( void *pSemaphore ){
	
	while(1){
		xSemaphoreGive(*((xSemaphoreHandle*)pSemaphore));
		vTaskDelay(1000);
	}
}

int main( void ) {

	xSemaphoreHandle xSemaphore;
	
	Led_Init();
	vSemaphoreCreateBinary(xSemaphore);
	xTaskCreate(Pulse_LED0, NULL , 100 , &xSemaphore , 2 , NULL );
	xTaskCreate(PulseTrigger, NULL , 100 , &xSemaphore, 1 , NULL );
	vTaskStartScheduler();

	while(1) {};
}
