#include "serwo.h"
#include <LPC21xx.H>
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#define mDETEKTORPIN 1<<10
#define DESIREDOFFSET 50
enum ServoState {CALLIB, IDDLE, IN_PROGRESS, OFFSET};
unsigned char ucFreqParameter, ucFrequency;

struct Servo{
	enum ServoState eState;
	unsigned int uiCurrentPosition;
	unsigned int uiDesiredPosition;
};

struct Servo eServo;

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
	while(1){
		
		ucFrequency = *((unsigned char*)pvParameters);
		
		switch(eServo.eState){
				case CALLIB:
					if(eReadDetector()==ACTIVE){
						eServo.uiCurrentPosition = 0;
						eServo.eState=OFFSET;
					}
					else{
						Led_StepLeft();
						eServo.eState = CALLIB;
					}
					break;
				case OFFSET:
					if(eServo.uiCurrentPosition > DESIREDOFFSET ){
						eServo.uiDesiredPosition = 0;
						eServo.uiCurrentPosition = 0;
						eServo.eState = IDDLE;
					}
					else{
						Led_StepRight();
						eServo.uiCurrentPosition++;
						eServo.eState = OFFSET;
					}
					break;
				case IN_PROGRESS:
					if(eServo.uiCurrentPosition == eServo.uiDesiredPosition){
						eServo.eState=IDDLE;
					}
					else if(eServo.uiCurrentPosition < eServo.uiDesiredPosition){
						Led_StepRight();
						eServo.uiCurrentPosition++;
						eServo.eState=IN_PROGRESS;
					}
					else {
						Led_StepLeft();
						eServo.uiCurrentPosition--;
						eServo.eState=IN_PROGRESS;
					}
					break;
				case IDDLE:
					if(eServo.uiCurrentPosition!=eServo.uiDesiredPosition){
						eServo.eState=IN_PROGRESS;
					} 
					else{
						eServo.eState=IDDLE;
					}
					break;		
			}
			vTaskDelay(ucFrequency);
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
	eServo.eState=IN_PROGRESS;
	eServo.uiDesiredPosition=uiPosition;
	while(eServo.eState==IN_PROGRESS){}
}
