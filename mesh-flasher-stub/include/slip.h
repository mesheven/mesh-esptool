#ifndef SLIP_H_
#define SLIP_H_

#include <stdint.h>


#define SLIP_UART_MAX_PACKET_LENGTH 1056   //Max effect data is 1040 = 8 + 8 + 1024(payload) 

enum {
    SLIP_STATUS_IDLE    = 0,
    SLIP_STATUS_MESSAGE = 1,
    SLIP_STATUS_ESC     = 2
};


typedef void (*slip_write_t)(uint8_t);

typedef struct
{
    uint32_t status;
    uint32_t index;
    uint8_t  buffer[SLIP_UART_MAX_PACKET_LENGTH];
} slip_rx_t;

typedef struct
{
    slip_write_t slip_write;
} slip_tx_t;

void slipRxUartInit(slip_rx_t* ctx);
void slipTxUartInit(slip_tx_t* ctx, slip_write_t write);

uint32_t slipUartProcess(slip_rx_t* ctx, uint8_t ch);

void slipSendStreamBeging(slip_tx_t* ctx);
void slipSendStream(slip_tx_t* ctx, const uint8_t* data, uint16_t length);
void slipSendStreamByte(slip_tx_t* ctx, uint8_t data);
void slipSendStreamEnd(slip_tx_t* ctx);
void slipSend(slip_tx_t* ctx, uint8_t* data, uint16_t length);


#endif /* SLIP_H_ */
