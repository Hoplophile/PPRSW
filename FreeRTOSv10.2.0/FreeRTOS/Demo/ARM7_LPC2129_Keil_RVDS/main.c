#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void LedBlink( void *pvParameters ){
	
	unsigned char ucFreq = *((unsigned char*)pvParameters);

	while(1){
		Led_Toggle(0);
		vTaskDelay((1000/ucFreq)/2);
	}
}

int main( void ) {

	unsigned char ucBlinkingFreq = 1;
	
	Led_Init();
	xTaskCreate(LedBlink, NULL , 100 , &ucBlinkingFreq, 2 , NULL );
	vTaskStartScheduler();

	while(1) {};
}
