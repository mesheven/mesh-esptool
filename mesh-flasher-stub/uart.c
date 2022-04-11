/*
 * Description: the UART driver routines
 *
 */
#include <stdint.h>
#include "rom_functions.h"
#include "uart.h"
#include "uart_register.h"
#include "eagle_soc.h"
#include "eagle_soc_ext.h"

/*
*    Note: in boot loader call_user_start: the system clock is 52Mhz, not 80Mhz
*/
#define UART_FLASH_CLK_FREQ   (52*1000000)

/******************************************************************************
 * FunctionName : uart_config
 * Description  : Internal used function
 *                UART0 used for data TX/RX, RX buffer size is 0x100, interrupt disable
 *                UART1 just used for debug output(Not in this routine)
 * Parameters   : bautrate for UART0
 * Returns      : NONE
*******************************************************************************/
void uart_config(uint8_t uart_no, UartConfig *pconfig)
{
    uint32_t  conf0 = 0;
    
    if(uart_no == UART0)
    {
        //
        PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);

        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
        if(pconfig->hwflowctrl == USART_HardwareFlowControl_CTS_RTS)
        {
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_UART0_CTS);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_UART0_RTS);
        }
        else if(pconfig->hwflowctrl == USART_HardwareFlowControl_CTS)
        {
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_UART0_CTS);
        }
        else if(pconfig->hwflowctrl == USART_HardwareFlowControl_RTS)
        {
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_UART0_RTS);
        }

        /*swap pins
            U0TXD=>pin:MTDO
            U0RXD=>pin:MTCK
            U0CTS=>pin:U0RXD
            U0RTS=>pin:U0TXD
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_U0CTS);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_U0RTS);
            SET_PERI_REG_MASK(0x3ff00028 , BIT2);
            */
    }
    else if(uart_no == UART1)
    {
        //select GPIO2 as UART1 Tx Pin
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);        
    }

    //set baudrate
    uart_div_modify(uart_no, UART_FLASH_CLK_FREQ / (pconfig->baudrate));

    //set data bits, only need support 7 and 8 bits
    if(pconfig->databit == EIGHT_BITS)
    {
        conf0 =  (3 << UART_BIT_NUM_S);    
    }
    else
    {
        conf0 =  (2 << UART_BIT_NUM_S);      
    }

    //set stop bits
    if(pconfig->stopbit == ONE_STOP_BIT)
    {
        conf0 |=  (1 << UART_STOP_BIT_NUM_S);    
    }
    else if(pconfig->stopbit == ONE_HALF_STOP_BIT)
    {
        conf0 |=  (2 << UART_STOP_BIT_NUM_S);      
    }
    else if(pconfig->stopbit == TWO_STOP_BIT)
    {
        conf0 |=  (3 << UART_STOP_BIT_NUM_S);      
    }      
        
    //set parity bits
    if(pconfig->paritybit == EVEN_BITS)
    {
        conf0 |=  UART_PARITY_EN;    
    }
    else if(pconfig->paritybit == ODD_BITS)
    {
        conf0 |=  UART_PARITY_EN | UART_PARITY;      
    }
    else if(pconfig->paritybit == NONE_BITS)
    {
        //      
    }          
    //write the data/stop/parity bits
    WRITE_PERI_REG(UART_CONF0(uart_no), conf0);

    
    //set Tx hardware flow control
    if((pconfig->hwflowctrl == USART_HardwareFlowControl_CTS_RTS) || (pconfig->hwflowctrl == USART_HardwareFlowControl_CTS))
    {
        SET_PERI_REG_MASK(UART_CONF0(uart_no), UART_TX_FLOW_EN);
    }
    

    //clear rx and tx fifo,not ready
    SET_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);
    CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);

    //set rx 3/4 FIFO trigger, 2 byte time out
    WRITE_PERI_REG(UART_CONF1(uart_no),
                   ((0x60 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |                
                   (0x02 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S |
                   UART_RX_TOUT_EN);
    //set  Rx hardware flow control
    if((pconfig->hwflowctrl == USART_HardwareFlowControl_CTS_RTS) || (pconfig->hwflowctrl == USART_HardwareFlowControl_RTS))
    {
        SET_PERI_REG_MASK(UART_CONF1(uart_no), ((0x60 & UART_RX_FLOW_THRHD) << UART_RX_FLOW_THRHD_S) | UART_RX_FLOW_EN );
    }    
    

    //clear all interrupt
    WRITE_PERI_REG(UART_INT_CLR(uart_no), 0xffff);
    
    //disable rx interrupt
    SET_PERI_REG_MASK(UART_INT_ENA(uart_no), 0x0000 );
}




/******************************************************************************
 * FunctionName : uart1_tx_one_char
 * Description  : Internal used function
 *                Use uart1 interface to transfer one char
 * Parameters   : uint8 TxChar - character to tx
*******************************************************************************/
void uart_tx_one_char(uint8_t uart, uint8_t TxChar)
{
    while (1)
    {
      //get Tx FIFO counter
      uint32_t fifo_cnt = (READ_PERI_REG(UART_STATUS(uart)) >> UART_TXFIFO_CNT_S) & UART_TXFIFO_CNT ;
      if (fifo_cnt  < 126) {
        break;
      }
    }

    WRITE_PERI_REG(UART_FIFO(uart) , TxChar);
}

/******************************************************************************
 * FunctionName : uart1_write_char
 * Description  : Internal used function
 *                Do some special deal while tx char is '\r' or '\n'
 * Parameters   : char c - character to tx
 * Returns      : NONE
*******************************************************************************/
void uart1_write_char(char c)
{
  if (c == '\n')
  {
    uart_tx_one_char(UART1, '\r');
    uart_tx_one_char(UART1, '\n');
  }
  else if (c == '\r')
  {
  }
  else
  {
    uart_tx_one_char(UART1, c);
  }
}

/******************************************************************************
 * FunctionName : uart1_write_char
 * Description  : Internal used function
 *                Do some special deal while tx char is '\r' or '\n'
 * Parameters   : char c - character to tx
 * Returns      : NONE
*******************************************************************************/
void uart0_write_char(char c)
{
    uart_tx_one_char(UART0, c);
}

/******************************************************************************
 * FunctionName : uart0_read_char
 * Description  :    read a char from UART0
 * Parameters : None
 * Returns       :  char
*******************************************************************************/
char uart0_read_char(void)
{
    char   rc = 0;
    while(((READ_PERI_REG(UART_STATUS(UART0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT) == 0x00);
    rc = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
    return rc;
}


/******************************************************************************
 * FunctionName : uart_init
 * Description  : user interface for init uart
 * Parameters   : UartBaudRate uart0_br - uart0 bautrate
 * Returns      : NONE
*******************************************************************************/
void uart_init(UartBaudRate uart0_br)
{
    UartConfig uartconfig;
    // rom use 74880 baut_rate, here reinitialize. 
    uartconfig.baudrate   =  uart0_br;
    uartconfig.databit    =  EIGHT_BITS;
    uartconfig.stopbit    =  ONE_STOP_BIT;
    uartconfig.paritybit  =  NONE_BITS;
    uartconfig.hwflowctrl =  USART_HardwareFlowControl_CTS_RTS;    

    //UART0 setting
    uart_config(UART0, &uartconfig);
    
    // disable UART interrupt
    ets_isr_mask(0x01 << 5);
}


