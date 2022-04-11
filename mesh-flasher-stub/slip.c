#include <stdlib.h>
#include <string.h>

#include "slip.h"


#define SLIP_END    (0xC0)
#define SLIP_ESC    (0xDB)
#define ESC_END     (0xDC)
#define ESC_ESC     (0xDD)

void slipRxUartInit(slip_rx_t* ctx)
{
    ctx->status = SLIP_STATUS_IDLE;
    ctx->index  = 0;    
    memset(ctx->buffer, 0x00, SLIP_UART_MAX_PACKET_LENGTH);
}

void slipTxUartInit(slip_tx_t* ctx, slip_write_t write)
{
    ctx->slip_write = write;
}


uint32_t slipUartProcess(slip_rx_t* ctx, uint8_t data)
{
    uint32_t ret = 0;
    //-- Process single byte
    switch(ctx->status)
    {
    //-----------------------------------------------------------
    case SLIP_STATUS_IDLE:
        if (data == SLIP_END)
        {
            ctx->index = 0;
            ctx->status = SLIP_STATUS_MESSAGE;
        }
        break;

    //-----------------------------------------------------------
    case SLIP_STATUS_MESSAGE:
        switch (data)
        {
        case SLIP_END:
            if (ctx->index == 0)
            {
                // 0xC0C0 on the bus.
            }
            else
            {
                // End of packet, return message
                ret = ctx->index;

                //start next packet
                ctx->index = 0; 
                ctx->status = SLIP_STATUS_MESSAGE;                
            }
            break;
        case SLIP_ESC:
            // Escape character, drop one byte and process next
            ctx->status = SLIP_STATUS_ESC;
            break;
        default:
            if (ctx->index < SLIP_UART_MAX_PACKET_LENGTH)
            {
                ctx->buffer[ctx->index++] = data;
            }
            else
            {
                ctx->status = SLIP_STATUS_IDLE;
            }
            break;
        }
        break;
    //-----------------------------------------------------------
    case SLIP_STATUS_ESC:
        switch (data)
        {
        case ESC_END:
            ctx->buffer[ctx->index++] = SLIP_END;
            ctx->status = SLIP_STATUS_MESSAGE;
            break;
        case ESC_ESC:
            ctx->buffer[ctx->index++] = SLIP_ESC;
            ctx->status = SLIP_STATUS_MESSAGE;
            break;
        case SLIP_END:
            //start next packet
            ctx->index = 0; 
            ctx->status = SLIP_STATUS_MESSAGE;   
            break;
        default:
            //-- Packet error!
            ctx->status = SLIP_STATUS_IDLE;
            break;
        }
        break;
    }
    //-----------------------------------------------------------
    return ret;
}

void slipSendStreamBeging(slip_tx_t* ctx)
{
    ctx->slip_write(SLIP_END);
}

void slipSendStreamEnd(slip_tx_t* ctx)
{
    ctx->slip_write(SLIP_END);
}

void slipSendStream(slip_tx_t* ctx, const uint8_t* data, uint16_t length)
{
    while(length--)
    {
        if (*data == SLIP_END)
        {
            ctx->slip_write(SLIP_ESC);
            ctx->slip_write(ESC_END);
        }
        else if (*data == SLIP_ESC)
        {
            ctx->slip_write(SLIP_ESC);
            ctx->slip_write(ESC_ESC);
        }
        else
        {
            ctx->slip_write(*data);
        }
        data++;
    }
}

void slipSendStreamByte(slip_tx_t* ctx, uint8_t data)
{
    slipSendStream(ctx, &data, 1);
}

void slipSend(slip_tx_t* ctx, uint8_t* data, uint16_t length)
{
    slipSendStreamBeging(ctx);
    slipSendStream(ctx, data, length);
    slipSendStreamEnd(ctx);
}

