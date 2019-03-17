enum eStan {ACTIVE, INACTIVE};

void DetectorInit(void);
enum eStan eReadDetector(void);
void Servo_Init(unsigned int uiServoFrequency);
void Servo_GoTo(unsigned int uiPosition);
void Servo_Callib(void);
