# bootup
bootup is a bootloader updater to help update flash-resident STM32F10x bootloaders that weren't written to handle self-updates.

Once loaded into the device via the existing bootloader, bootup copies its code into RAM and reflashes the device with the updated bootloader firmware.

The payload size can be larger than the original bootloader - bootup will take care of copying the flash pages in the correct order. This can be useful to update the bootloader and the firmware in a single step. bootup itself only adds about 3KiB of overhead.

## Configuration
The default linker script assumes your bootloader loads the application at 0x08002000 and that the bootloader resides at 0x08000000.

To override the application address, modify the linker script [stm32f103x8-ram-2k.ld](src/stm32f103x8-ram-2k.ld)

To override the bootloader address, define the `PAYLOAD_TARGET` symbol.

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
All contents of the bootup project are licensed under terms that are compatible with the terms of the GNU Lesser General Public License version 3.

Non-libopencm3 related portions of the bootup project are licensed under the less restrictive ISC license, except where otherwise specified in the headers of specific files.

See the LICENSE file for full details.
