/*
 * Copyright (c) 2017, Devan Lai
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* Common STM32F103 target functions */

#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/desig.h>

#include "target.h"
#include "config.h"

#define FLASH_OPTION_BYTE_RDP FLASH_OPTION_BYTE(0)
#define FLASH_PAGE_SIZE_HALF_WORDS ((FLASH_PAGE_SIZE)/2)

void target_clock_setup(void) {
    /* Set system clock to 48 MHz */
    rcc_clock_setup_in_hsi_out_48mhz();
}

void target_gpio_setup(void) {
    /* Enable GPIO clocks */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

    /* Setup LEDs */
#if HAVE_LED
    {
        const uint8_t mode = GPIO_MODE_OUTPUT_10_MHZ;
        const uint8_t conf = (LED_OPEN_DRAIN ? GPIO_CNF_OUTPUT_OPENDRAIN
                                             : GPIO_CNF_OUTPUT_PUSHPULL);
        if (LED_OPEN_DRAIN) {
            gpio_clear(LED_GPIO_PORT, LED_GPIO_PIN);
        } else {
            gpio_set(LED_GPIO_PORT, LED_GPIO_PIN);
        }
        gpio_set_mode(LED_GPIO_PORT, mode, conf, LED_GPIO_PIN);
    }
#endif
}

void target_reset_self(void) {
    scb_reset_system();
    while (1);
}

void target_disable_write_protection(void) {    
    flash_unlock_option_bytes();
    flash_erase_option_bytes();

    if (FLASH_OBR & FLASH_OBR_RDPRT_EN) {
        // This is not going to end well...
    } else {
        flash_program_option_bytes(FLASH_OPTION_BYTE_RDP, FLASH_RDP_KEY);
    }

    flash_lock();
    target_reset_self();
}

void target_flash_unlock(void) {
    if (FLASH_WRPR != 0xFFFFFFFFUL) {
        target_disable_write_protection();
    }
    
    flash_unlock();
}

void target_flash_lock(void) {
    flash_lock();
}

static inline uint16_t* get_flash_page_address(const uint16_t* dest) {
    return (uint16_t*)(((uint32_t)dest / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE);
}

uint16_t* target_get_flash_page_address(const uint16_t* dest) {
    return get_flash_page_address(dest);
}

static uint16_t* get_flash_start(void) {
    return (uint16_t*)(FLASH_BASE);
}

static uint16_t* get_flash_end(void) {
#ifdef FLASH_SIZE_OVERRIDE
    /* Allow access to the unofficial full 128KiB flash size */
    return (uint16_t*)(FLASH_BASE + FLASH_SIZE_OVERRIDE);
#else
    /* Only allow access to the chip's self-reported flash size */
    return (uint16_t*)(FLASH_BASE + (size_t)DESIG_FLASH_SIZE*FLASH_PAGE_SIZE);
#endif
}

bool target_flash_program_array(uint16_t* dest, const uint16_t* data, size_t half_word_count) {
    bool verified = true;

    /* Remember the bounds of erased data in the current page */
    static uint16_t* erase_start;
    static uint16_t* erase_end;

    /* Avoid writing before the start of flash */
    if (dest < get_flash_start()) {
        return false;
    }

    const uint16_t* flash_end = get_flash_end();
    while (half_word_count > 0) {
        /* Avoid writing past the end of flash */
        if (dest >= flash_end) {
            verified = false;
            break;
        }

        if (dest >= erase_end || dest < erase_start) {
            erase_start = get_flash_page_address(dest);
            erase_end = erase_start + FLASH_PAGE_SIZE_HALF_WORDS;
            flash_erase_page((uint32_t)erase_start);
        }
        flash_program_half_word((uint32_t)dest, *data);
        erase_start = dest + 1;
        if (*dest != *data) {
            verified = false;
            break;
        }
        dest++;
        data++;
        half_word_count--;
    }

    return verified;
}

void target_flash_erase_to_end(uint16_t* start) {
    uint16_t* erase_start = get_flash_page_address(start);
    uint16_t* flash_end = get_flash_end();
    while (erase_start < flash_end) {
        flash_erase_page((uint32_t)erase_start);
        erase_start += FLASH_PAGE_SIZE_HALF_WORDS;
    }
}

static inline void write_led(bool state) {
#if HAVE_LED
    if ((bool)LED_OPEN_DRAIN ^ state) {
        gpio_set(LED_GPIO_PORT, LED_GPIO_PIN);
    } else {
        gpio_clear(LED_GPIO_PORT, LED_GPIO_PIN);
    }
#else
    (void)state;
#endif
}

static inline void blink(int count, int ontime, int offtime) {
#if HAVE_LED
    write_led(false);
    int i;
    for(i=0; i < count; i++) {
        gpio_toggle(LED_GPIO_PORT, LED_GPIO_PIN);
        int j;
        for (j=0; j < 24000*ontime; j++) {
            __asm__("nop");
        }
        gpio_toggle(LED_GPIO_PORT, LED_GPIO_PIN);
        for (j=0; j < 24000*offtime; j++) {
            __asm__("nop");
        }
    }
#else
    (void)count;
    (void)ontime;
    (void)offtime;
#endif
}

void target_update_status(enum Status status) {
    switch (status) {
        case STATUS_START:
            break;
        case STATUS_RUNNING:
            write_led(true);
            break;
        case STATUS_DONE:            
            blink(3, 100, 100);
            break;
        default:
            blink(3, 500, 500);
            break;
    }
}
