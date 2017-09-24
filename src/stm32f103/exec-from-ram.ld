/*
 * This file is part of the bootup project.
 *
 * Copyright (C) 2017 Devan Lai <devan.lai@gmail.com>
 * Copyright (C) 2015 Karl Palsson <karlp@tweak.net.au>
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Common linker script settings to execute most code from RAM */


/* Enforce emission of the vector table. */
EXTERN (vector_table)

/* Define the entry point of the output file. */
ENTRY(reset_handler)

/* Define sections. */
SECTIONS
{
	/* Vector table */
	.vectors : {
		*(.vectors)
	} >rom

	/* Place rom->ram loader code in rom */
	.loader : {
		. = ALIGN(4);
		*libopencm3_stm32f1.a:vector.o(.text* .rodata*)
	} >rom

    /* Post-loader startup code can execute from rom */

	/* C++ Static constructors/destructors, also used for __attribute__
	 * ((constructor)) and the likes */
	.preinit_array : {
		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;
	} >rom
	.init_array : {
		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;
	} >rom

	/* Technically, this is post-startup code, but we should never
       actually get to the point where this runs */
	.fini_array : {
		. = ALIGN(4);
		__fini_array_start = .;
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		__fini_array_end = .;
	} >rom

	/* All post-startup code must execute from ram, since flash get erased */
	.text : {
		_data = .;
		*(.text*)	/* Program code */
		. = ALIGN(4);
    	/* Read-only data to read from ram, except for the payload */
		EXCLUDE_FILE (payload.o) *(.rodata*)
		. = ALIGN(4);
	} >ram AT> rom

	_data_loadaddr = LOADADDR(.text);

	/*
	 * Another section used by C++ stuff, appears when using newlib with
	 * 64bit (long long) printf support
	 */
	.ARM.extab : {
		*(.ARM.extab*)
	} >ram AT> rom
	.ARM.exidx : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >ram AT> rom

	. = ALIGN(4);
	_etext = .;

	.data : {
		*(.data*)	/* Read-write initialized data */
		. = ALIGN(4);
		_edata = .;
	} >ram AT >rom

	/* Payload stored in flash, aligned to a page boundary to simplify
       the overlapping flash moving code */
	.payload : {
		. = ALIGN(1024);
		payload.o(.rodata*)
	} >rom

	.bss : {
		*(.bss*)	/* Read-write zero initialized data */
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} >ram

	/*
	 * The .eh_frame section appears to be used for C++ exception handling.
	 * You may need to fix this if you're using C++.
	 */
	/DISCARD/ : { *(.eh_frame) }

	. = ALIGN(4);
	end = .;
}

PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));
