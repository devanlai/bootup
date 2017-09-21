#!/usr/bin/env python

import argparse
import math
import sys
import zlib
import struct

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "input",
        type=argparse.FileType("rb"),
        help="Input file to compress")
    parser.add_argument(
        "--reverse",
        action="store_true",
        help="Reverse the input before compressing and reverse the compressed output")
    parser.add_argument(
        "--window-size",
        type=int,
        choices=[2**W for W in range(9, 15)],
        default=2**15,
        help="Sliding window size in bytes")
    parser.add_argument(
        "-v", "--verbose",
        action="store_true")

    args = parser.parse_args()

    # Set the window size and specify no gzip header/trailer
    wbits = -int(round(math.log(args.window_size, 2)))
    compressor = zlib.compressobj(9, zlib.DEFLATED, wbits, 9)

    contents = args.input.read()
    uncompressed_crc32 = zlib.crc32(contents) & 0xFFFFFFFF

    if args.reverse:
        # Reverse the contents and compress them, then reverse that
        # This guarantees that the last byte of the input is the
        # first byte to be compressed and that the last byte
        # of the output is the first byte to be decompressed
        # Note that the decompressor will need to operate backwards,
        # starting from high memory addresses and growing down
        compressed = (compressor.compress(contents[::-1]) + compressor.flush())[::-1]
    else:        
        compressed = (compressor.compress(contents) + compressor.flush())

    compressed_crc32 = zlib.crc32(compressed) & 0xFFFFFFFF

    # Write out CRC32s for compressed and original contents, both in little-endian
    sys.stdout.write(struct.pack("<LL", compressed_crc32, uncompressed_crc32))

    # Write out the decompressed size in little-endian
    sys.stdout.write(struct.pack("<L", len(contents)))
    
    # Write out the compressed image
    sys.stdout.write(compressed)

    if args.verbose:
        sys.stderr.write("Original size: %10d\n" % len(contents))
        sys.stderr.write("Deflated size: %10d\n" % len(compressed))
        sys.stderr.write("Original CRC:  0x%8X\n" % uncompressed_crc32)
        sys.stderr.write("Deflated CRC:  0x%8X\n" % compressed_crc32)
