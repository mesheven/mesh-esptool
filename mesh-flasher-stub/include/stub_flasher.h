#ifndef STUB_FLASHER_H_
#define STUB_FLASHER_H_

#include <stdint.h>


/* Flash geometry constants */
#define FLASH_SECTOR_SIZE   (4096)
#define FLASH_BLOCK_SIZE    (65536)
#define FLASH_PAGE_SIZE     (256)
#define FLASH_STATUS_MASK   (0xFFFF)
#define SECTORS_PER_BLOCK   (FLASH_BLOCK_SIZE / FLASH_SECTOR_SIZE)

#define FLASH_MAX_SIZE      (16*1024*1024)

/* Full set of protocol commands */
typedef enum {
    ESP_SYNC               = 0x08,
    ESP_ERASE_FLASH_CHIP   = 0x09,
    ESP_ERASE_FLASH_SECTOR = 0x0A,
    ESP_PROGRAM_FLASH      = 0x0B,
    ESP_READ_FLASH         = 0x0C,
} esp_command;


/* Error codes */
typedef enum {
    ESP_OK                    = 0,
    
    ESP_BAD_PARAMETERS        = 5,
    ESP_BAD_COMMAND           = 6,
    ESP_BAD_CRC               = 7,
    ESP_PROGRAM_FLASH_ERROR   = 8,
    ESP_READ_FLASH_ERROR      = 9,
    ESP_BAD_READ_FLASH_LENGTH = 10,
    ESP_ERASE_CHIP_ERROR      = 11,
    ESP_ERASE_SECTOR_ERROR    = 12,
    
} esp_command_error;


/* Command request header */
typedef struct __attribute__((packed)) {
    uint8_t     zero;
    uint8_t     op;         // maps to esp_command enum
    uint16_t    data_len;
    uint32_t    checksum;
    uint32_t    parameter1;
    uint32_t    parameter2;
    uint8_t     data_buf[1]; // actually variable length, determined by data_len
} esp_command_req_t;

/* Command response header */
typedef struct __attribute__((packed)) {
    uint8_t     resp;       // should be '1' 
    uint8_t     op_ret;     // Should match 'op' 
    uint16_t    len_ret;    // Length of result data (can be ignored as SLIP framing helps)
    uint32_t    value;      //  32-bit response used by some commands 
    uint8_t     error;      // non-zero = failed 
    uint8_t     status;     // status of a failure 
} esp_command_response_t;

#define RESPONSE_BODY_LEN       (2)
#define RESPONSE_COMMON_LEN     (10)

#endif /* STUB_FLASHER_H_ */
