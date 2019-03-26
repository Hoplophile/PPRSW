#include "FreeRTOS.h"

enum eStan {ACTIVE, INACTIVE};

void DetectorInit(void);
enum eStan eReadDetector(void);
void Servo_Init(void);
void Servo_GoTo(unsigned int uiPosition);
void Servo_Callib(void);
void Servo_Wait(TickType_t TicksWait);
void Servo_Speed(unsigned char ucServoSpeed);
void Automat(void*);
