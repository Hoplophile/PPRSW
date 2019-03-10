#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

struct BlinkParams {
	unsigned char ucFrequency;
	unsigned char ucLedIndex;
} sBlinkParams;

void LedBlink( void *pvParameters ){

	while(1){
		Led_Toggle((*((struct BlinkParams*)pvParameters)).ucLedIndex);
		vTaskDelay((1000/((*((struct BlinkParams*)pvParameters)).ucFrequency))/2);
	}
}

void LedCtrl( void *pvFreqParam ){
	
	while(1){
		(*((struct BlinkParams*)pvFreqParam)).ucFrequency = (*((struct BlinkParams*)pvFreqParam)).ucFrequency + 1;
		vTaskDelay(1000);
		(*((struct BlinkParams*)pvFreqParam)).ucFrequency = (*((struct BlinkParams*)pvFreqParam)).ucFrequency + 1;
		if((*((struct BlinkParams*)pvFreqParam)).ucLedIndex == 3) (*((struct BlinkParams*)pvFreqParam)).ucLedIndex = 0;
		else	(*((struct BlinkParams*)pvFreqParam)).ucLedIndex = (*((struct BlinkParams*)pvFreqParam)).ucLedIndex + 1;
		vTaskDelay(1000);
	}
}

int main( void ) {

	sBlinkParams.ucFrequency = 1;
	sBlinkParams.ucLedIndex = 0;
	
	Led_Init();
	xTaskCreate(LedBlink, NULL , 100 , &sBlinkParams, 1 , NULL );
	xTaskCreate(LedCtrl, NULL , 100 , &sBlinkParams, 2 , NULL );
	vTaskStartScheduler();

	while(1) {};
}
