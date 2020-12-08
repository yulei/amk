
import sys
import struct

VECTOR_FMT = "<7I"
CHECKSUM_FMT = "<1I"
CHECKSUM_START = 0x1C

BL_START = 0x08010000
BL_SIZE = 0x0000FC00
BL_INFO_OFFSET = 0x20
BL_INFO_SIZE = 12

DAPLINK_BUILD_KEY_IF = 0x9B939E8F
DAPLINK_BUILD_KEY_BL = 0x9B939D93
DAPLINK_HIC_ID_STM32F103XB = 0x97969908
DAPLINK_VERSION=254

#info consist of key, hic id, version
BL_INFO = []

def patch_crc(data):
    import binascii
    crc_size = len(data) - 4
    new_data = data[:crc_size]
    crc32 = binascii.crc32(new_data) & 0xFFFFFFFF

    print("crc32: {}".format(hex(crc32)))

    # Write CRC to the file in little endian
    new_crc = bytes([(crc32 >> 0) & 0xFF,(crc32 >> 8) & 0xFF, (crc32 >> 16) & 0xFF,(crc32 >> 24) & 0xFF ])
    return new_data+new_crc

def patch_data(data):
    vectors = struct.unpack(VECTOR_FMT, data[0 : 4 * 7])
    checksum = 0
    for vector in vectors:
        checksum += vector
    checksum = (~checksum + 1) & 0xFFFFFFFF  # Two's compliment
    csum_data = struct.pack(CHECKSUM_FMT, checksum)

    info = (DAPLINK_BUILD_KEY_BL).to_bytes(4,'little')
    info += (DAPLINK_HIC_ID_STM32F103XB).to_bytes(4,'little')
    info += (DAPLINK_VERSION).to_bytes(4,'little')
    return patch_crc(data[0:CHECKSUM_START] + csum_data + info + data[BL_INFO_OFFSET+BL_INFO_SIZE:])

def patch_file(in_file, out_file, hex_file):
    with open(in_file, 'rb') as i:
        data = i.read()
        padding = bytes([0xFF])
        data = data.ljust(BL_SIZE, padding)
        data = patch_data(data)
        with open(out_file, 'wb') as df:
            df.write(data)

        if len(hex_file) > 0:
            from intelhex import IntelHex
            out = IntelHex()
            out.frombytes(data, offset=BL_START)
            out.write_hex_file(hex_file)

def usage():
    print("patch_file <in> <out> [hex]")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        usage()
    else:
        patch_file(sys.argv[1], sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else "")
