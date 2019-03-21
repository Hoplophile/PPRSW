#include "serwo.h"
#include <LPC21xx.H>
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define mDETEKTORPIN 1<<10
#define DESIREDOFFSET 50
enum ServoState {CALLIB, IDDLE, IN_PROGRESS, OFFSET};
unsigned char ucFreqParameter, ucFrequency;
unsigned int uiBufferPosition;
QueueHandle_t xPositionsQueue;

struct Servo{
	enum ServoState eState;
	unsigned int uiPosition;
};

struct Servo eServo, eServoTemp;

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
	
	xPositionsQueue = xQueueCreate(20, 2*sizeof(int));
	
	while(1){
		
		ucFrequency = *((unsigned char*)pvParameters);
		xQueuePeek(xPositionsQueue, &uiBufferPosition, portMAX_DELAY);
		
		switch(eServo.eState){
				case CALLIB:
					if(eReadDetector()==ACTIVE){
						eServo.uiPosition = 0;
						eServo.eState=OFFSET;
					}
					else{
						Led_StepLeft();
						eServo.eState = CALLIB;
					}
					break;
				case OFFSET:
					if(eServo.uiPosition > DESIREDOFFSET ){
						eServo.uiPosition = 0;
						eServo.eState = IDDLE;
					}
					else{
						Led_StepRight();
						eServo.uiPosition++;
						eServo.eState = OFFSET;
					}
					break;
				case IN_PROGRESS:
					if(eServo.uiPosition == uiBufferPosition){
						xQueueReceive(xPositionsQueue, NULL, portMAX_DELAY);
						eServo.eState=IDDLE;
					}
					else if(eServo.uiPosition < uiBufferPosition){
						Led_StepRight();
						eServo.uiPosition++;
						eServo.eState=IN_PROGRESS;
					}
					else {
						Led_StepLeft();
						eServo.uiPosition--;
						eServo.eState=IN_PROGRESS;
					}
					break;
				case IDDLE:
					if(eServo.uiPosition!=uiBufferPosition){
						eServo.eState=IN_PROGRESS;
					} 
					else{
						eServo.eState=IDDLE;
					}
					break;		
			}
			vTaskDelay(1);
		}
	}
void Servo_Init(unsigned int uiServoFrequency){
	DetectorInit();
	eServo.eState=CALLIB;
	Led_Init();
	ucFreqParameter = (1000000/uiServoFrequency);
	xTaskCreate(Automat, NULL, 127, &ucFreqParameter, 2, NULL);
}
void Servo_Callib(void){
	eServo.eState=CALLIB;
}
void Servo_GoTo(unsigned int uiPosition){
	eServoTemp.eState=IN_PROGRESS;
	eServoTemp.uiPosition = uiPosition;
	xQueueSend(xPositionsQueue, &eServoTemp, portMAX_DELAY);
	while(eServo.eState==IN_PROGRESS){}
}
