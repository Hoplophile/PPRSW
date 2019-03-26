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
					Servo_Speed(8);
					Servo_GoTo(12);
					Servo_Wait(100);
					Servo_Speed(4);
					Servo_GoTo(24);
					Servo_Wait(100);
					Servo_Speed(2);
					Servo_GoTo(36);
					Servo_Wait(100);
					Servo_Speed(1);
					Servo_GoTo(0);
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
	Servo_Init();
	
	xTaskCreate(Automat, NULL, 127, NULL, 1, NULL);
	xTaskCreate(Keyboard, NULL, 127, NULL, 2, NULL);
	vTaskStartScheduler();
	while(1){};
}
