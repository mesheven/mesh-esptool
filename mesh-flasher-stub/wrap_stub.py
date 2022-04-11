#!/usr/bin/env python2
#
# Stub has to be generated via Python 3, for correct repr() output
#
# SPDX-FileCopyrightText: 2016 Cesanta Software Limited
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD

from __future__ import division, print_function

import argparse
import base64
import os
import os.path
import re
import sys
import zlib
import struct

sys.path.append("..")
import esptool  # noqa: E402


def wrap_stub(elf_file):
    """Wrap an ELF file into a stub 'dict'"""
    print("Wrapping ELF file %s..." % elf_file)
    e = esptool.bin_image.ELFFile(elf_file)

    text_section = e.get_section(".text")
    try:
        data_section = e.get_section(".data")
    except ValueError:
        data_section = None
    stub = {
        "text": text_section.data,
        "text_start": text_section.addr,
        "entry": e.entrypoint,
    }
    if data_section is not None:
        stub["data"] = data_section.data
        stub["data_start"] = data_section.addr

    # Pad text with NOPs to mod 4.
    if len(stub["text"]) % 4 != 0:
        stub["text"] += (4 - (len(stub["text"]) % 4)) * "\0"

    print(
        "Stub text: %d @ 0x%08x, data: %d @ 0x%08x, entry @ 0x%x"
        % (
            len(stub["text"]),
            stub["text_start"],
            len(stub.get("data", "")),
            stub.get("data_start", 0),
            stub["entry"],
        ),
        file=sys.stderr,
    )
    return stub

def section_out(dataCode):
    words = (len(dataCode) + 3) // 4
    ret = "const uint32_t length = {}; \n".format(words)
    lines = len(dataCode) // 32
    lastlinebytes = len(dataCode) % 32

    for i in range(lines):
        buf32 = dataCode[0:32]
        dataCode = dataCode[32:]
        values = struct.unpack_from('<IIIIIIII', buf32)
        for j in range(8):
            strtex = "0x{value:0>8X}, ".format(value = values[j])
            ret = ret + strtex
        ret = ret + "\n"

    #Last lines
    if ((lastlinebytes % 4) != 0):
        dataCode += (4 - lastlinebytes % 4) * b"\0"
    for i in range((lastlinebytes + 3) // 4):
        buf4 = dataCode[0:4]
        dataCode = dataCode[4:]
        values = struct.unpack_from('<I', buf4)
        strtex = "0x{value:0>8X}, ".format(value = values[0])
        ret = ret + strtex
    ret = ret + "\n"
    
    return ret

def write_image_to_file(stub_dict, out_file):
    for name, stub_data in stub_dict.items():
        textAddress = stub_data["text_start"]
        textEntry   = stub_data["entry"]
        dataAddress = stub_data["data_start"]
        textCode    = stub_data["text"]
        dataCode    = stub_data["data"]
        strtex = "text_start = 0x{addr:0>8X}".format(addr = textAddress)
        out_file.write(strtex + "\n")
        strtex = "text entry = 0x{addr:0>8X}".format(addr = textEntry)
        out_file.write(strtex + "\n")
        strtex = "data_start = 0x{addr:0>8X}".format(addr = dataAddress)
        out_file.write(strtex + "\n")
        out_file.write("Text Code:" + "\n")
        strtex = section_out(textCode);
        out_file.write(strtex);
        out_file.write("Data Code:" + "\n")
        strtex = section_out(dataCode);
        out_file.write(strtex);


def stub_name(filename):
    """Return a dictionary key for the stub with filename 'filename'"""
    return os.path.splitext(os.path.basename(filename))[0]


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--out-file",
        required=False,
        type=argparse.FileType("w"),
        help="Output file name",
    )
    parser.add_argument("elf_files", nargs="+", help="Stub ELF files to convert")
    args = parser.parse_args()

    stubs = dict(
        (stub_name(elf_file), wrap_stub(elf_file)) for elf_file in args.elf_files
    )
    if args.out_file:
        print("Dumping to file %s." % args.out_file.name)
        write_image_to_file(stubs, args.out_file)
    else:
        print("missig output file name")
