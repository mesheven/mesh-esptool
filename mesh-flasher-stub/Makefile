#
# Makefile for mesh-flasher-stub
#


AR = xtensa-lx106-elf-ar
CC = xtensa-lx106-elf-gcc
LD = xtensa-lx106-elf-gcc
NM = xtensa-lx106-elf-nm
CPP = xtensa-lx106-elf-cpp
OBJCOPY = xtensa-lx106-elf-objcopy
OBJDUMP = xtensa-lx106-elf-objdump

# Python command to invoke wrap_stub.py
WRAP_STUB ?= ./wrap_stub.py

STUB = stub_flasher
SRCS = stub_flasher.c slip.c uart.c

CFLAGS    = -std=c99 -Wall -Werror -Os -O3 -mtext-section-literals -mlongcalls -nostdlib -fno-builtin -flto -Wl,-static -g -ffunction-sections -Wl,--gc-sections -Iinclude -Lld
LDFLAGS   = -lgcc

BUILD_DIR = build

STUB_ELF_8266 = $(BUILD_DIR)/$(STUB)_8266.elf
STUB_PY = $(BUILD_DIR)/$(STUB)_image.txt

.PHONY: all clean

all: $(BUILD_DIR) $(STUB_PY)

$(BUILD_DIR):
	@mkdir -p $@

$(STUB_ELF_8266): $(SRCS)
	@echo "  CC(8266)   $^ -> $@"
	@$(CC) $(CFLAGS) -Tstub_8266.ld -Wl,-Map=$(@:.elf=.map) -o $@ $(filter %.c, $^) $(LDLIBS)

$(STUB_PY): $(STUB_ELF_8266) wrap_stub.py
	@echo "  WRAP $^ -> $@"
	@$(WRAP_STUB) --out-file $@ $(filter %.elf,$^)
    
clean:
	@echo "RM $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)
