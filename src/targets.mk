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

ifeq ($(TARGET),STM32F103)
	TARGET_COMMON_DIR   := ./stm32f103
	TARGET_SPEC_DIR     := ./stm32f103/generic
	ARCH                 = STM32F1
endif
ifeq ($(TARGET),STLINK)
	TARGET_COMMON_DIR   := ./stm32f103
	TARGET_SPEC_DIR     := ./stm32f103/generic
	ARCH                 = STM32F1
    DEFS                += -DHAVE_LED=1 -DLED_GPIO_PORT=GPIOA -DLED_GPIO_PIN=GPIO9 -DLED_OPEN_DRAIN=0
endif
ifeq ($(TARGET),BLUEPILL)
	TARGET_COMMON_DIR   := ./stm32f103
	TARGET_SPEC_DIR     := ./stm32f103/generic
	ARCH                 = STM32F1
	DEFS                += -DHAVE_LED=1 -DLED_GPIO_PORT=GPIOC -DLED_GPIO_PIN=GPIO13 -DLED_OPEN_DRAIN=1
endif
ifeq ($(TARGET),MAPLEMINI)
	TARGET_COMMON_DIR   := ./stm32f103
	TARGET_SPEC_DIR     := ./stm32f103/generic
	ARCH                 = STM32F1
	DEFS                += -DHAVE_LED=1 -DLED_GPIO_PORT=GPIOB -DLED_GPIO_PIN=GPIO1 -DLED_OPEN_DRAIN=0
endif

ifeq ($(BOOTUP_OFFSET),0)
	LDSCRIPT            := ./stm32f103/stm32f103x8.ld
else
	LDSCRIPT            := ./stm32f103/stm32f103x8-$(BOOTUP_OFFSET).ld
endif

LDFLAGS += -L$(TARGET_COMMON_DIR)

ifndef ARCH
$(error Unknown target $(TARGET))
endif
