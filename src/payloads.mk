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

PAYLOAD_DIR ?= payloads

ifdef PAYLOAD
ifdef PAYLOAD_BIN_SRC
$(error Must set only one of PAYLOAD or PAYLOAD_BIN_SRC)
endif
endif

PAYLOAD ?= DAPBOOT

ifndef PAYLOAD_BIN_SRC
ifeq ($(PAYLOAD),DAPBOOT)
ifeq ($(TARGET),STLINK)
	PAYLOAD_BIN_SRC = $(PAYLOAD_DIR)/dapboot-v1.10-stlink.bin
else ifeq ($(TARGET),BLUEPILL)
	PAYLOAD_BIN_SRC = $(PAYLOAD_DIR)/dapboot-v1.10-bluepill.bin
else ifeq ($(TARGET),STM32F103)
	PAYLOAD_BIN_SRC = $(PAYLOAD_DIR)/dapboot-v1.10-bluepill.bin
else ifeq ($(TARGET),MAPLEMINI)
	PAYLOAD_BIN_SRC = $(PAYLOAD_DIR)/dapboot-v1.10-maplemini.bin
else
$(error No known $(PAYLOAD) payload for $(TARGET))
endif
endif
endif

ifdef PAYLOAD_BIN_SRC
payload.bin: $(PAYLOAD_BIN_SRC)
	@echo Using payload $(PAYLOAD_BIN_SRC)
	$(Q)cp $(PAYLOAD_BIN_SRC) $@
endif

ifndef PAYLOAD
ifndef PAYLOAD_BIN_SRC
$(error Must set PAYLOAD to payload name or PAYLOAD_BIN_SRC to bin path)
endif
endif
