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

#include "target.h"
#include "config.h"
#include "payload.h"

/* dest and src must both be page aligned */
static bool flash_move(uint16_t* dest, const uint16_t* src, size_t size) {
    /* no-op case */
    if (dest == src || size == 0) {
        return true;
    }

    const size_t hw_count = (size + 1) / 2;
    uint16_t* dest_end_page = target_get_flash_page_address(dest + hw_count);
    uint16_t* src_end_page = target_get_flash_page_address(src + hw_count);

    /* Check if we need to copy from the end first */
    if (dest <= src_end_page && src_end_page < dest_end_page) {
        uint16_t* src_page = src_end_page;
        uint16_t* dest_page = dest_end_page;
        size_t page_hw_count = (dest + hw_count) - dest_page;

        do {
            bool ok = target_flash_program_array(dest_page, src_page, page_hw_count);
            dest_page -= page_hw_count;
            src_page  -= page_hw_count;
            page_hw_count = (FLASH_PAGE_SIZE) / 2;

            if (!ok) {
                return false;
            }
        } while (dest_page >= dest);
        return true;
    } else {
        return target_flash_program_array(dest, src, hw_count);
    }
}

int main(void) {
    target_clock_setup();
    target_gpio_setup();
    target_update_status(STATUS_START);

    target_flash_unlock();
    const uint16_t* payload = (const uint16_t*)_binary_payload_bin_start;
    const size_t payload_size = _binary_payload_bin_end - _binary_payload_bin_start;

    target_update_status(STATUS_RUNNING);

    // Copy the payload to its target address
    uint16_t* dest = (uint16_t*)(PAYLOAD_TARGET);
    bool ok = flash_move(dest, payload, payload_size);

    if (ok) {
        // Erase everything following the payload
        uint16_t* first_page_after_payload =
            target_get_flash_page_address(dest + (payload_size + FLASH_PAGE_SIZE - 1)/2);
        target_flash_erase_to_end(first_page_after_payload);
    }

    target_flash_lock();

    if (ok) {
        target_update_status(STATUS_DONE);
    } else {
        target_update_status(STATUS_ERROR);
    }
    
    target_reset_self();

    while (1);
    
    return 0;
}
