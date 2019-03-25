#include "serwo.h"
#include <LPC21xx.H>
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define mDETEKTORPIN 1<<10

enum ServoState {CALLIB, IDDLE, IN_PROGRESS};
QueueHandle_t xPositionsQueue;

struct Servo{
	enum ServoState eState;
	unsigned int uiPosition;
	unsigned int uiDesiredPosition;
};

void DetectorInit(){
	IO0DIR = IO0DIR & (~(mDETEKTORPIN));
}

enum eStan eReadDetector(){
	if((0==(IO0PIN & (mDETEKTORPIN)))){
		return ACTIVE;
	}
	else{
		return INACTIVE;
	}
}
void Automat(void *pvTickPeriod){
	
	static struct Servo sServo = {CALLIB, 0, 0};
	
	while(1){
		
		switch(sServo.eState){
				case CALLIB:
					if(eReadDetector()==ACTIVE){
						sServo.uiPosition = 0;
						sServo.uiDesiredPosition = 0;
						sServo.eState=IDDLE;
					}
					else{
						Led_StepLeft();
						sServo.eState = CALLIB;
					}
					break;
				case IN_PROGRESS:
					if(sServo.uiPosition > sServo.uiDesiredPosition){
						Led_StepLeft();
						sServo.uiPosition--;
						sServo.eState=IN_PROGRESS;
					}
					else if(sServo.uiPosition < sServo.uiDesiredPosition){
						Led_StepRight();
						sServo.uiPosition++;
						sServo.eState=IN_PROGRESS;
					}
					else {
						sServo.eState=IDDLE;
					}
					break;
				case IDDLE:
					if(sServo.uiPosition != sServo.uiDesiredPosition){
						sServo.eState=IN_PROGRESS;
					} 
					else {
						struct Servo sServoBuffer = {IDDLE, 0, 0};
						if (xQueueReceive(xPositionsQueue, &sServoBuffer, 0) == pdPASS) {
							sServoBuffer.eState = sServoBuffer.eState;
							sServo.uiDesiredPosition = sServoBuffer.uiDesiredPosition;
						}
						else {
							sServo.eState=IDDLE;
						}
					}
					break;		
			}
			vTaskDelay(*((TickType_t*)pvTickPeriod));
		}
	}
void Servo_Init(void){
	
	xPositionsQueue = xQueueCreate(20, sizeof(struct Servo));
	DetectorInit();
	Led_Init();
	Servo_Callib();
}
void Servo_Callib(void){
	
	struct Servo sServo = {CALLIB, 0, 0};
	xQueueSend(xPositionsQueue, &sServo, 0);
}
void Servo_GoTo(unsigned int uiPosition){
	
	struct Servo sServo;
	sServo.eState=IN_PROGRESS;
	sServo.uiDesiredPosition = uiPosition;
	xQueueSend(xPositionsQueue, &sServo, 0);
}
