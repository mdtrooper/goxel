#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import json
import struct

def insert_chunk(gox_bin, chunk):
    [gox_bin.append(x) for x in chunk.type.encode()]
    temp = bytearray()
    if type(chunk.data) == bytes or type(chunk.data) == bytearray:
        temp.extend(chunk.data)
    gox_bin.extend(struct.pack('<i', len(temp)))
    gox_bin.extend(temp)
    gox_bin.extend(struct.pack('<i', 0)) # crc

def dictTobytearray(dictionary):
    return_b = bytearray()
    
    
    
    return return_b

def main(inputfile, outputfile):
    gox_bin = bytearray()
    gox_json = dict();
    with open(inputfile) as f:
        gox_json = json.load(f)
    
    # Header
    [gox_bin.append(x) for x in 'GOX '.encode()]
    gox_bin.extend(struct.pack('<i', gox_json['header']['version']))
    
    # Image
    
    
    print(gox_bin)
    with open(outputfile, 'wb') as f:
        f.write(gox_bin)
    

if __name__ == "__main__":
    inputfile = sys.argv[1]
    outputfile = sys.argv[2]
    main(inputfile, outputfile)
