#include "serwo.h"
#include <LPC21xx.H>
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define mDETEKTORPIN 1<<10

enum ServoState {CALLIB, IDDLE, IN_PROGRESS, WAIT, SPEED};
QueueHandle_t xServoQueue;

struct Servo{
	enum ServoState eState;
	unsigned int uiPosition;
	unsigned int uiDesiredPosition;
	TickType_t TicksWait;
	unsigned char ucServoSpeed;
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
void Automat(void *pvParameters){
	
	static struct Servo sServo = {CALLIB, 0, 0, 0, 10};
	
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
						struct Servo sServoBuffer = {IDDLE, 0, 0, 0, 0};
						if (xQueueReceive(xServoQueue, &sServoBuffer, 0) == pdPASS) {
							if(sServoBuffer.eState == WAIT){
								vTaskDelay(sServoBuffer.TicksWait);
								sServo.eState = IDDLE;
							} else if(sServoBuffer.eState == SPEED) {
								sServo.ucServoSpeed = sServoBuffer.ucServoSpeed;
							} else {
							sServoBuffer.eState = sServoBuffer.eState;
							sServo.uiDesiredPosition = sServoBuffer.uiDesiredPosition;
							}
						} else {
							sServo.eState=IDDLE;
						}
					}
					break;
				case WAIT:
				case SPEED:
					break;
			}
			vTaskDelay((TickType_t)sServo.ucServoSpeed);
		}
	}
void Servo_Init(void){
	
	xServoQueue = xQueueCreate(20, sizeof(struct Servo));
	DetectorInit();
	Led_Init();
	Servo_Callib();
}
void Servo_Callib(void){
	
	struct Servo sServo = {CALLIB, 0, 0, 0, 0};
	xQueueSend(xServoQueue, &sServo, 0);
}
void Servo_GoTo(unsigned int uiPosition){
	
	struct Servo sServo;
	sServo.eState=IN_PROGRESS;
	sServo.uiDesiredPosition = uiPosition;
	xQueueSend(xServoQueue, &sServo, 0);
}

void Servo_Wait(TickType_t TicksWait){
	
	struct Servo sServo = {WAIT, 0, 0, 0, 0};
	sServo.TicksWait = TicksWait;
	xQueueSend(xServoQueue, &sServo, 0);
}

void Servo_Speed(unsigned char ucServoSpeed){
	
	struct Servo sServo = {SPEED, 0, 0, 0, 0};
	sServo.ucServoSpeed = ucServoSpeed;
	xQueueSend(xServoQueue, &sServo, 0);
}
