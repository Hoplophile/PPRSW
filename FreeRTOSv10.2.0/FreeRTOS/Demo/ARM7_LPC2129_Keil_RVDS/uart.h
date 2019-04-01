#define UART_RX_BUFFER_SIZE  15
#define UART_TX_BUFFER_SIZE 15

enum eRecieverStatus { EMPTY, READY, OVERFLOW};
enum eTransmiterStatus {FREE, BUSY};

void UART_InitWithInt(unsigned int uiBaudRate);
void Reciever_PutCharacterToBuffer(char cCharacter);
enum eRecieverStatus eUartRx_GetStatus(void);
void Uart_GetStringCopy(char * ucDestination);

void Transmiter_SendString(char cString[]);
enum eTransmiterStatus eUartTx_GetStatus(void);
