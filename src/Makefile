## Copyright (c) 2017, Devan Lai
##
## Permission to use, copy, modify, and/or distribute this software
## for any purpose with or without fee is hereby granted, provided
## that the above copyright notice and this permission notice
## appear in all copies.
##
## THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
## WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
## WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
## AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
## CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
## LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
## NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
## CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-include local.mk

BINARY = bootup
OPENCM3_DIR = ../libopencm3

TARGET ?= STM32F103
BOOTUP_OFFSET ?= 8192

include targets.mk
include payloads.mk
include uzlib.mk

SRCS := $(wildcard *.c)
SRCS += $(wildcard $(TARGET_COMMON_DIR)/*.c)
SRCS += $(wildcard $(TARGET_SPEC_DIR)/*.c)

OBJS += $(SRCS:.c=.o)
DEPS  = $(SRCS:.c=.d)

OBJS += payload.o

.DEFAULT_GOAL := $(BINARY).bin

payload.o: payload.bin
	$(Q)$(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata $^ $@

include rules.mk

size: $(OBJS) $(BINARY).elf
	@$(PREFIX)-size $(OBJS) $(BINARY).elf

debug: $(BINARY).elf
	-$(GDB) --tui --eval "target remote | $(OOCD) -f $(OOCD_INTERFACE) -f $(OOCD_BOARD) -f debug.cfg" $(BINARY).elf

erase:
	$(OOCD) -f $(OOCD_INTERFACE) -f $(OOCD_BOARD) \
            -c "init" -c "reset init" \
            -c "stm32f1x unlock 0; reset halt" \
            -c "flash erase_sector 0 0 last" -c "reset" -c "shutdown"

clean::
	@rm -f $(OBJS)
	@rm -f $(DEPS)

.PHONY += debug size erase

OBJS := $(sort $(OBJS))

# Add the base directory to the header search path
CPPFLAGS += -I.

# Add target config directory to the header search path
CPPFLAGS += -I$(TARGET_COMMON_DIR)/
CPPFLAGS += -I$(TARGET_SPEC_DIR)/

# Pass the payload target address through, if provided
ifdef PAYLOAD_TARGET
	DEFS += -DPAYLOAD_TARGET=$(PAYLOAD_TARGET)
endif
