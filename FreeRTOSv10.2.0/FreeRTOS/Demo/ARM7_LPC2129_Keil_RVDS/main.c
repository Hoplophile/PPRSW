#include "FreeRTOS.h"
#include "task.h"
#include "keyboard.h"
#include "serwo.h"

enum KeyboardState ePreviousState = RELASED, eCurrentState = RELASED;

void Keyboard (void *pvParameters){
	
	while(1){			
		ePreviousState = eCurrentState;
		eCurrentState = eKeyboard_Read();
		
		if (ePreviousState != eCurrentState){
			switch(eCurrentState){
				case BUTTON_1: 
					Servo_Callib();
					break;
				case BUTTON_2: 
					Servo_GoTo(50);
					break;
				case BUTTON_3: 
					Servo_GoTo(100);
					break;
				case BUTTON_4: 
					Servo_GoTo(150);
					break;
				default:
					break;
			}		
		}
		vTaskDelay(1);
	}
}

int main( void ){
	
	ButtonInit();
	Servo_Init(100);
	
	xTaskCreate(Keyboard, NULL, 127, NULL, 1, NULL);
	vTaskStartScheduler();
	while(1){};

}
