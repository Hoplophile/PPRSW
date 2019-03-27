#include "serwo.h"
#include <LPC21xx.H>
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define mDETEKTORPIN 1<<10

enum ServoState {CALLIB, IDDLE, IN_PROGRESS};
enum ServoCommand {CALLIB_COMMAND, GOTO_COMMAND, WAIT_COMMAND, SPEED_COMMAND};
QueueHandle_t xServoQueue;

struct Servo{
	enum ServoState eState;
	unsigned int uiCurrentPosition;
	unsigned int uiDesiredPosition;
	TickType_t TicksWait;
	unsigned char ucServoSpeed;
};

struct ServoCom{
	enum ServoCommand eCommand;
	unsigned int uiArgument;
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
						sServo.uiCurrentPosition = 0;
						sServo.uiDesiredPosition = 0;
						sServo.eState=IDDLE;
					}
					else{
						Led_StepLeft();
						sServo.eState = CALLIB;
					}
					break;
				case IN_PROGRESS:
					if(sServo.uiCurrentPosition > sServo.uiDesiredPosition){
						Led_StepLeft();
						sServo.uiCurrentPosition--;
						sServo.eState=IN_PROGRESS;
					}
					else if(sServo.uiCurrentPosition < sServo.uiDesiredPosition){
						Led_StepRight();
						sServo.uiCurrentPosition++;
						sServo.eState=IN_PROGRESS;
					}
					else {
						sServo.eState=IDDLE;
					}
					break;
				case IDDLE:
				{
					struct ServoCom sServoBuffer = {GOTO_COMMAND, 0};
					
					if (xQueueReceive(xServoQueue, &sServoBuffer, portMAX_DELAY) == pdPASS) {
						switch(sServoBuffer.eCommand){
							case WAIT_COMMAND:
								vTaskDelay((TickType_t)sServoBuffer.uiArgument);
								sServo.eState = IDDLE;
								break;
							case SPEED_COMMAND:
								sServo.ucServoSpeed = sServoBuffer.uiArgument;
								sServo.eState = IDDLE;
								break;
							case GOTO_COMMAND:
								sServo.uiDesiredPosition = sServoBuffer.uiArgument;
								sServo.eState = IN_PROGRESS;
								break;
							case CALLIB_COMMAND:
								sServo.eState = CALLIB;
								break;
						}
					}
					break;
				}
			}
			vTaskDelay((TickType_t)sServo.ucServoSpeed);
		}
	}
void Servo_Init(void){
	
	xServoQueue = xQueueCreate(20, sizeof(struct ServoCom));
	DetectorInit();
	Led_Init();
	Servo_Callib();
}
void Servo_Callib(void){
	
	struct Servo sServo = {CALLIB, 0, 0, 0, 0};
	xQueueSend(xServoQueue, &sServo, 0);
}
void Servo_GoTo(unsigned int uiPosition){
	
	struct ServoCom sServoCommand;
	sServoCommand.eCommand=GOTO_COMMAND;
	sServoCommand.uiArgument = uiPosition;
	xQueueSend(xServoQueue, &sServoCommand, 0);
}

void Servo_Wait(TickType_t TicksWait){
	
	struct ServoCom sServoCommand;
	sServoCommand.eCommand = WAIT_COMMAND;
	sServoCommand.uiArgument = TicksWait;
	xQueueSend(xServoQueue, &sServoCommand, 0);
}

void Servo_Speed(unsigned char ucServoSpeed){
	
	struct ServoCom sServoCommand;
	sServoCommand.eCommand = SPEED_COMMAND;
	sServoCommand.uiArgument = ucServoSpeed;
	xQueueSend(xServoQueue, &sServoCommand, 0);
}
