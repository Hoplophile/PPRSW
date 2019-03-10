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

void TaskSuspender ( void *pHandle ){
	
	TaskHandle_t thTask = *((TaskHandle_t*)pHandle); 
	
	while(1){
		vTaskSuspend(thTask);
		vTaskDelay(1000);
		vTaskResume(thTask);
		vTaskDelay(1000);
	}
}

int main( void ) {

	TaskHandle_t thBlinkHandle;
	sBlinkParams.ucFrequency = 10;
	sBlinkParams.ucLedIndex = 0;
	
	Led_Init();
	xTaskCreate(LedBlink, NULL , 100 , &sBlinkParams, 1 , &thBlinkHandle );
	xTaskCreate(TaskSuspender, NULL , 100 , &thBlinkHandle, 2 , NULL );
	vTaskStartScheduler();

	while(1) {};
}
