#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import json
import struct

def insert_chunk(gox_bin, chunk):
    [gox_bin.append(x) for x in chunk['type'].encode()]
    temp = bytearray()
    if type(chunk['data']) == bytes or type(chunk['data']) == bytearray:
        temp.extend(chunk['data'])
    gox_bin.extend(struct.pack('<i', len(temp)))
    gox_bin.extend(temp)
    gox_bin.extend(struct.pack('<i', 0)) # crc

def dictsTobytearray(dicts):
    return_b = bytearray()
    for key, value in dicts.items():
        temp = bytearray()
        [temp.append(x) for x in key.encode()]
        return_b.extend(struct.pack('<i', len(temp)))
        return_b.extend(temp)
        return_b.extend(struct.pack('<i', len(value)))
        return_b.extend(value)
    return return_b

def streamhex2bytearray(streamhex):
    return_b = bytearray()
    for i in range(0, len(streamhex), 2):
        return_b.append(int(streamhex[i:(i+2)], 16))
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
    chunk = dict()
    chunk['type'] = 'IMG '
    chunk['data'] = dictsTobytearray({'box': streamhex2bytearray(gox_json['image']['box'])})
    insert_chunk(gox_bin, chunk)
    
    # BL16
    chunk = dict()
    chunk['type'] = 'BL16'
    chunk['data'] = dictsTobytearray({'box': streamhex2bytearray('ffff')})
    insert_chunk(gox_bin, chunk)
    
    print(gox_bin)
    with open(outputfile, 'wb') as f:
        f.write(gox_bin)

if __name__ == "__main__":
    inputfile = sys.argv[1]
    outputfile = sys.argv[2]
    main(inputfile, outputfile)
