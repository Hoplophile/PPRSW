#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "timer_interrupts.h"
#include "semphr.h"
	
extern SemaphoreHandle_t LedSemaphore;

void LedBlink(void *pvParameters)
{
	while(1){
		xSemaphoreTake(LedSemaphore, portMAX_DELAY);
		Led_Toggle(0);
	}
}

int main(void) {
	
	Led_Init();
	Timer1Interrupts_Init(50000);
	xTaskCreate(&LedBlink, NULL, 127, NULL, 8, NULL);
	vTaskStartScheduler();
	
	while(1) {};
}
