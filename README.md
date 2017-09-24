# bootup
bootup is a bootloader updater to help update flash-resident STM32F10x bootloaders that weren't written to handle self-updates.

Once loaded into the device via the existing bootloader, bootup copies its code into RAM and reflashes the device with the updated bootloader firmware.

The payload size can be larger than the original bootloader - bootup will take care of copying the flash pages in the correct order. This can be useful to update the bootloader and the firmware in a single step. bootup itself only adds about 8KiB of overhead.

# Compression
On this experimental branch, the payload is compressed during the build process and decompressed by bootup at runtime.
This allows bootup to deliver firmware updates that are too big for the existing bootloader.

The final decompressed image may overlap with bootup's compressed firmware image.
bootup decompresses the image into flash starting from the end, which minimizes the number of pages of compressed data that need to be backed up to RAM during decompression.

    0
    +------------------+ <- Start reading here
    | compressed image |
    +------------------+
    +------------------------------------------------------------------+ <- Start writing here
    | decompressed image                                               |
    +------------------------------------------------------------------+

In practice, the compressed image will be offset from the final decompressed image destination, because the bootloader and the bootup firmware takes up space:

    0            0x2000   0x4000
    +------------+--------+------------------+ <- Start reading here
    | bootloader | bootup | compressed image |
    +------------+--------+------------------+
    +------------------------------------------------------------------+ <- Start writing here
    | decompressed image                                               |
    +------------------------------------------------------------------+

As long as the offset is small, bootup can handle the extra overlap by buffering those extra pages to RAM.

## Configuration
The default linker script assumes your bootloader loads the application at 0x08002000 and that the bootloader resides at 0x08000000.

To override the address that bootup will run from, define `BOOTUP_OFFSET` to select the linker script:

    make BOOTUP_OFFSET=16384

To override the address that bootup will write the new bootloader to, define the `PAYLOAD_TARGET` symbol, e.g:

    make PAYLOAD_TARGET=0x08002000

The `TARGET` variable sets the hardware platform, which primarily determines which LEDs will blink.

The `PAYLOAD_BIN_SRC` variable can be used to specify an external firmware `.bin` file to load.

## Build instructions
In general, you can build an updater using make and specifying your target and payload path:

    make clean
    make TARGET=STM32F103 PAYLOAD_BIN_SRC=/my/new/bootloader.bin

To build a [dapboot](https://github.com/devanlai/dapboot) updater, you can instead specify `PAYLOAD=DAPBOOT`:

    make clean
    make TARGET=BLUEPILL PAYLOAD=DAPBOOT
    dfu-util -d 1209:db42 -D bootup.bin

### Hardware Targets
| Target Name | Description | Link |
| ----------- | ----------- |----- |
|`STM32F103`  | Generic target | |
|`BLUEPILL`   | Cheap dev board | http://wiki.stm32duino.com/index.php?title=Blue_Pill |
|`MAPLEMINI`  | LeafLabs Maple Mini board and clone derivatives | http://wiki.stm32duino.com/index.php?title=Maple_Mini |
|`STLINK`     | STLink/v2 hardware clones | https://wiki.paparazziuav.org/wiki/STLink#Clones |

## Overriding defaults
Local makefile settings can be set by creating a `local.mk`, which is automatically included.

Here is an example `local.mk` that overrides the default hardware target and payload file

    TARGET ?= STLINK
    PAYLOAD_BIN_SRC ?= /my/new/firmware.bin

## Licensing
The core bootup code is licensed under the ISC license.

uzlib is licensed under... I'm not sure - consult its README and headers for details.

libopencm3 related portions of the bootup project are licensed under the LGPLv3.

See the LICENSE file for full details.
