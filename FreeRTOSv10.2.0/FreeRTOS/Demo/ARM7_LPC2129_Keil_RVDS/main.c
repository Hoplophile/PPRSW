#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void LedBlink( void *pvParameters ){

	while(1){
		Led_Toggle(0);
		vTaskDelay((1000/(*((unsigned char*)pvParameters)))/2);
	}
}

void LedCtrl( void *pvFreqParam ){
	
	while(1){
		*(unsigned char*)pvFreqParam = *((unsigned char*)pvFreqParam) + 1;
		vTaskDelay(1000);
	}
}

int main( void ) {

	unsigned char ucBlinkingFreq = 1;
	
	Led_Init();
	xTaskCreate(LedBlink, NULL , 100 , &ucBlinkingFreq, 1 , NULL );
	xTaskCreate(LedCtrl, NULL , 100 , &ucBlinkingFreq, 1 , NULL );
	vTaskStartScheduler();

	while(1) {};
}
