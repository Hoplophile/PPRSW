#define RECIEVER_SIZE  30
#define TRANSMITER_SIZE 30

enum eRecieverStatus { EMPTY, READY, OVERFLOW};
enum eTransmiterStatus {FREE, BUSY};

void UART_InitWithInt(unsigned int uiBaudRate);
void Reciever_PutCharacterToBuffer(char cCharacter);
enum eRecieverStatus eReciever_GetStatus(void);
void Reciever_GetStringCopy(char * ucDestination);

void Transmiter_SendString(char cString[]);
enum eTransmiterStatus Transmiter_GetStatus(void);
