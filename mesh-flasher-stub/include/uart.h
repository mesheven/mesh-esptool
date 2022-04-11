/*
 * Description:
 *         define UART using MACROs. include data/stop/party bits, bautrate....
 */
#ifndef UART_APP_H
#define UART_APP_H

#include <stdint.h>
#include "uart_register.h"
#include "eagle_soc.h"



#define UART0   0
#define UART1   1


typedef enum {
    BIT_RATE_300     = 300,
    BIT_RATE_600     = 600,
    BIT_RATE_1200    = 1200,
    BIT_RATE_2400    = 2400,
    BIT_RATE_4800    = 4800,
    BIT_RATE_9600    = 9600,
    BIT_RATE_19200   = 19200,
    BIT_RATE_38400   = 38400,
    BIT_RATE_57600   = 57600, 
    BIT_RATE_74880   = 74880,
    BIT_RATE_115200  = 115200,
    BIT_RATE_230400  = 230400,
    BIT_RATE_460800  = 460800,
    BIT_RATE_921600  = 921600,
    BIT_RATE_1843200 = 1843200,
    BIT_RATE_3686400 = 3686400,
    BIT_RATE_1000000 = 1000000,
    BIT_RATE_2000000 = 2000000,     
    BIT_RATE_BOOT230400  = 230400,
    BIT_RATE_BOOT74880   = 74880,
    BIT_RATE_BOOT1000000  = 1000000, 
    BIT_RATE_BOOT2000000  = 2000000,   
} UartBaudRate;

typedef enum {
    EIGHT_BITS = 0,
    SEVEN_BITS = 1,
} UartDataBits;

typedef enum {
    ONE_STOP_BIT             = 0,
    ONE_HALF_STOP_BIT        = 1,
    TWO_STOP_BIT             = 2
} UartStopBits;


typedef enum {
    EVEN_BITS  = 0,
    ODD_BITS   = 1,
    NONE_BITS  = 2
} UartParityBits;

typedef enum {
    USART_HardwareFlowControl_None    = 0,
    USART_HardwareFlowControl_RTS     = 1,
    USART_HardwareFlowControl_CTS     = 2,
    USART_HardwareFlowControl_CTS_RTS = 3
} UARTHardwareFlowCtrl;


typedef struct {
    UartBaudRate         baudrate;
    UartDataBits         databit;
    UartStopBits         stopbit;
    UartParityBits       paritybit;
    UARTHardwareFlowCtrl hwflowctrl;
}UartConfig;



//#define MAX_RX_MESSAGE_LEN      512


void uart_init(UartBaudRate uart0_br);
void uart0_write_char(char c);
char uart0_read_char(void);




#endif

