#ifndef ROM_FUNCTIONS_H_
#define ROM_FUNCTIONS_H_

/* ROM function prototypes for functions in ROM which are
   called by the flasher stubs.
*/

#include <stdint.h>



typedef enum { 
    SPI_FLASH_RESULT_OK = 0,
    SPI_FLASH_RESULT_ERR = 1,
    SPI_FLASH_RESULT_TIMEOUT = 2 
} SpiFlashOpResult;

SpiFlashOpResult SPILock();
SpiFlashOpResult SPIUnlock();
SpiFlashOpResult SPIRead(uint32_t addr, void *dst, uint32_t size);
SpiFlashOpResult SPIWrite(uint32_t addr, const uint8_t *src, uint32_t size);
SpiFlashOpResult SPIEraseChip();
SpiFlashOpResult SPIEraseBlock(uint32_t block_num);
SpiFlashOpResult SPIEraseSector(uint32_t sector_num);

void spi_flash_attach();

void SelectSpiFunction();
uint32_t SPIParamCfg(uint32_t deviceId, uint32_t chip_size, uint32_t block_size, uint32_t sector_size, uint32_t page_size, uint32_t status_mask);


void ets_isr_mask(uint32_t ints);
void ets_isr_unmask(uint32_t ints);
void ets_set_user_start(void (*user_start_fn)());

void uart_div_modify(uint32_t uart_no, uint32_t baud_div);

int ets_printf(const char *format, ...);
void ets_install_putc1(void (*p)(char c));
void ets_delay_us(uint32_t us);

#endif // ROM_FUNCTIONS_H_

