#include <stdlib.h>
#include <string.h>
#include "rom_functions.h"
#include "slip.h"
#include "uart.h"
#include "stub_flasher.h"



static slip_rx_t  slipRxCtx;
static slip_tx_t  slipTxCtx;

static esp_command_response_t response;

static uint8_t  readBuffer[1024] __attribute__((aligned(16)));

static uint8_t calculate_checksum(uint8_t *buf, int length)
{
    uint8_t res = 0xef;
    for(int i = 0; i < length; i++) {
        res ^= buf[i];
    }
    return res;
}

static uint8_t verify_data_len(esp_command_req_t *pcommand, uint16_t len)
{
    return (pcommand->data_len == len) ? ESP_OK : ESP_BAD_PARAMETERS;
}

static void send_response(uint8_t *pbuffer, uint32_t length)
{
    slipSendStreamBeging(&slipTxCtx);
    slipSendStream(&slipTxCtx, pbuffer, length);
    slipSendStreamEnd(&slipTxCtx);
}

static void send_read_flash_response(uint8_t *pheader, uint32_t header_len, uint8_t *pbuffer, uint32_t buffer_len)
{
    slipSendStreamBeging(&slipTxCtx);
    slipSendStream(&slipTxCtx, pheader, header_len);
    slipSendStream(&slipTxCtx, pbuffer, buffer_len);
    slipSendStreamEnd(&slipTxCtx);
}

static void serial_write(uint8_t ch)
{
    uart0_write_char(ch);
}


static void init_response(esp_command_response_t *presponse, uint8_t op, uint16_t len, uint8_t error, uint8_t reason)
{
    presponse->resp    = 1;
    presponse->op_ret  = op;
    presponse->len_ret = len;
    presponse->value   = 0;
    presponse->error   = error;
    presponse->status  = reason;
}


static uint8_t handleEraseChip()
{
    SpiFlashOpResult result =  SPI_FLASH_RESULT_OK;
    result = SPIUnlock();
    if (result == SPI_FLASH_RESULT_OK)
    {
        result = SPIEraseChip();
    }
    return (result == SPI_FLASH_RESULT_OK) ? ESP_OK : ESP_ERASE_CHIP_ERROR;
}

static uint8_t handleEraseSectors(uint32_t address, uint32_t length)
{
    SpiFlashOpResult result = SPI_FLASH_RESULT_OK;
    uint32_t sectors   = 0;
    uint32_t sectorNum = 0;
    // align 
    sectors   = (length + FLASH_SECTOR_SIZE - 1)/ FLASH_SECTOR_SIZE;
    sectorNum = address / FLASH_SECTOR_SIZE;
    
    result = SPIUnlock();
    for (uint32_t i = 0; i < sectors; i++)
    {
        if (result == SPI_FLASH_RESULT_OK)
        {
            result = SPIEraseSector(sectorNum + i);
        }
    }
    // return 
    return (result == SPI_FLASH_RESULT_OK) ? ESP_OK : ESP_ERASE_SECTOR_ERROR;
}

static uint8_t handleProgramFlash(uint32_t address, uint8_t *pbuffer, uint32_t length)
{
    SpiFlashOpResult result = SPI_FLASH_RESULT_OK;
    result = SPIWrite(address, pbuffer, length);
    // return 
    return (result == SPI_FLASH_RESULT_OK) ? ESP_OK : ESP_PROGRAM_FLASH_ERROR;
}

static uint8_t handleReadFlash(uint32_t address, uint8_t *pbuffer, uint32_t length)
{
    SpiFlashOpResult result = SPI_FLASH_RESULT_OK;
    result = SPIRead(address, pbuffer, length);
    return (result == SPI_FLASH_RESULT_OK) ? ESP_OK : ESP_READ_FLASH_ERROR;
}


void cmd_loop()
{
    uint8_t data = 0;
    uint8_t result = ESP_OK;
    while (1)
    {
        // read data from uart, do slip
        data = uart0_read_char();
        uint32_t msglen = slipUartProcess(&slipRxCtx, data);
        // slip done
        if (msglen != 0)
        {
            esp_command_req_t *command = (esp_command_req_t *)slipRxCtx.buffer;
            switch(command->op)
            {
                case ESP_SYNC:
                result = verify_data_len(command, 36);
                if (result == ESP_OK)
                {
                    init_response(&response, ESP_SYNC, RESPONSE_BODY_LEN, 0, 0);
                }
                else
                {
                    init_response(&response, ESP_SYNC, RESPONSE_BODY_LEN, 1, ESP_BAD_PARAMETERS);
                }
                send_response((uint8_t *)&response, RESPONSE_COMMON_LEN);
                break;
                
                case ESP_ERASE_FLASH_CHIP:
                result = verify_data_len(command, 8);
                if (result == ESP_OK)
                {
                    result = handleEraseChip();
                }
                if (result == ESP_OK)
                {
                    init_response(&response, ESP_ERASE_FLASH_CHIP, RESPONSE_BODY_LEN, 0, 0);
                }
                else
                {
                    init_response(&response, ESP_ERASE_FLASH_CHIP, RESPONSE_BODY_LEN, 1, result);
                }
                send_response((uint8_t *)&response, RESPONSE_COMMON_LEN);
                break;

                case ESP_ERASE_FLASH_SECTOR:
                result = verify_data_len(command, 8);
                if (result == ESP_OK)
                {
                    result = handleEraseSectors(command->parameter1, command->parameter2);
                }
                if (result == ESP_OK)
                {
                    init_response(&response, ESP_ERASE_FLASH_SECTOR, RESPONSE_BODY_LEN, 0, 0);
                }
                else
                {
                    init_response(&response, ESP_ERASE_FLASH_SECTOR, RESPONSE_BODY_LEN, 1, result);
                }
                send_response((uint8_t *)&response, RESPONSE_COMMON_LEN);
                break;

                case ESP_PROGRAM_FLASH:
                result = verify_data_len(command, 8);
                if (result == ESP_OK)
                {
                    uint32_t checksum = calculate_checksum(command->data_buf, command->parameter2);
                    if (checksum == command->checksum) 
                    {
                        result = handleProgramFlash(command->parameter1, command->data_buf, command->parameter2);
                    }
                    else
                    {
                        result = ESP_BAD_CRC;
                    }
                }
                if (result == ESP_OK)
                {
                    init_response(&response, ESP_PROGRAM_FLASH, RESPONSE_BODY_LEN, 0, 0);
                }
                else
                {
                    init_response(&response, ESP_PROGRAM_FLASH, RESPONSE_BODY_LEN, 1, result);
                }
                send_response((uint8_t *)&response, RESPONSE_COMMON_LEN);
                break;
                
                case ESP_READ_FLASH:
                result = verify_data_len(command, 8);
                if (result == ESP_OK)
                {
                    result = handleReadFlash(command->parameter1, readBuffer, command->parameter2);
                    if (result == ESP_OK)
                    {
                        uint32_t checksum = calculate_checksum(readBuffer, command->parameter2);
                        init_response(&response, ESP_READ_FLASH, RESPONSE_BODY_LEN, 0, 0);
                        response.value = checksum; // modify checksum
                        send_read_flash_response((uint8_t *)&response, RESPONSE_COMMON_LEN, readBuffer, command->parameter2);
                    }
                }
                if (result != ESP_OK)
                {
                    init_response(&response, ESP_READ_FLASH, RESPONSE_BODY_LEN, 1, result);
                    send_response((uint8_t *)&response, RESPONSE_COMMON_LEN);
                }
                
                break;
                
                default:
                init_response(&response, command->op, RESPONSE_BODY_LEN, 1, ESP_BAD_COMMAND);
                send_response((uint8_t *)&response, RESPONSE_COMMON_LEN);
                break;
            }
        }
    }
    
}


extern uint32_t _bss_start;
extern uint32_t _bss_end;

void __attribute__((used)) stub_main();


__asm__ (
  ".global stub_main_8266\n"
  ".literal_position\n"
  ".align 4\n"
  "stub_main_8266:\n"
/* ESP8266 wrapper for "stub_main()" manipulates the return address in
 * a0, so 'return' from here runs user code.
 *
 * After setting a0, we jump directly to stub_main_inner() which is a
 * normal C function
 *
 * Adapted from similar approach used by Cesanta Software for ESP8266
 * flasher stub.
 *
 */
  "movi a0, 0x400010a8;"
  "j stub_main;");


/* This function is called from stub_main, with return address
   reset to point to user code. */
void stub_main()
{
    /* this points to stub_main now, clear for next boot */
    ets_set_user_start(0);

    /* zero bss */
    for(uint32_t *p = &_bss_start; p < &_bss_end; p++) {
        *p = 0;
    }

    // init slip
    slipRxUartInit(&slipRxCtx);
    slipTxUartInit(&slipTxCtx, serial_write);
    // init uart
    uart_init(BIT_RATE_1000000);

    /* Configure default SPI flash functionality.
     Can be overriden later by esptool.py. */
    SelectSpiFunction();
    spi_flash_attach();
    SPIParamCfg(0, FLASH_MAX_SIZE, FLASH_BLOCK_SIZE, FLASH_SECTOR_SIZE,
                    FLASH_PAGE_SIZE, FLASH_STATUS_MASK);

    cmd_loop();

    /* if cmd_loop returns, it's due to ESP_RUN_USER_CODE command. */

    return;
}
