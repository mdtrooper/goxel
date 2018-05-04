#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Translate a gox binary file to json file.

__author__ = "Miguel"
__copyright__ = "Copyright 2018"
__license__ = "GPL"

from sys import argv
from os import stat
from os.path import isfile
from subprocess import run
from struct import unpack

def help():
    print("Usage: {command} [arguments] file.gox".format(command=argv[0]))
    print()
    print("Arguments:")
    print("\t-h or --help\t Show this help")

def main():
    # It is better the getopts but it is ok at the moment.
    if any(_ in ['-h', '--help'] for _ in argv):
        help()
        exit(0)
    
    gox_filename = None
    
    for arg in argv[1:]:
        if isfile(arg):
            gox_filename = arg
            break
    
    if gox_filename:
        gox_to_json(gox_filename)
    else:
        print('ERROR: File not found.')
        help()
        exit(1)

def read_dict_from_data(data):
    size = int.from_bytes(data[:4], byteorder='little')
    data = data[4:]
    
    key = data[:size].decode()
    data = data[size:]
    
    size = int.from_bytes(data[:4], byteorder='little')
    data = data[4:]
    
    value = data[:size]
    data = data[size:]
    
    return data, {key: value}

def gox_to_json(filename):
    file_dict = {}
    file_dict['goxel_version'] = '0.7.3'
    try:
        with open(filename, 'rb') as f:
            file_dict['magic_string'] = f.read(4).decode()
            
            if file_dict['magic_string'] != 'GOX ':
                print('ERROR: The file is not a gox file.')
                exit(1)
            
            file_dict['version'] = int.from_bytes(f.read(4), byteorder='little')
            
            # READ CHUNKS
            file_dict['chunks'] = []
            while True:
                chunk = {}
                
                buff = f.read(4)
                if len(buff) == 0: # End of file
                    break
                if len(buff) < 4: # Error
                    raise Exception('Chunk is malformed')
                
                chunk['type'] = buff.decode().strip()
                size = int.from_bytes(f.read(4), byteorder='little')
                data = f.read(size)
                crc = int.from_bytes(f.read(4), byteorder='little')
                
                if chunk['type'] == 'IMG':
                    chunk['data'] = []
                    while len(data) > 0:
                        data, data_dict = read_dict_from_data(data)
                        chunk['data'].append(data_dict)
                if chunk['type'] == 'PREV':
                    chunk['data'] = data
                if chunk['type'] == 'BL16':
                    chunk['data'] = data
                if chunk['type'] == 'LAYR':
                    num_blocks = int.from_bytes(data[:4], byteorder='little')
                    data = data[4:]
                    
                    chunk['blocks'] = []
                    count_blocks = 0
                    while count_blocks < num_blocks:
                        index = int.from_bytes(data[:4], byteorder='little')
                        data = data[4:]
                        _x,_y,_z = unpack('<iii', data[:4*3])
                        print(_x,_y,_z)
                        x = int.from_bytes(data[:4], byteorder='little')
                        data = data[4:]
                        y = int.from_bytes(data[:4], byteorder='little')
                        data = data[4:]
                        z = int.from_bytes(data[:4], byteorder='little')
                        data = data[4:]
                        
                        data = data[4:] # Move a int32 ahead
                        
                        # TODO: Get the v from BL16
                        
                        chunk['blocks'].append({'x': x, 'y': y, 'z': z, 'v': None})
                        count_blocks += 1
                    
                    chunk['data'] = []
                    while len(data) > 0:
                        data, data_dict = read_dict_from_data(data)
                        key = list(data_dict.keys())[0]
                        if key in ['id', 'base_id']:
                            data_dict[key] = int.from_bytes(data_dict[key], byteorder='little')
                        if key in ['name']:
                            data_dict[key] = data_dict[key].decode()
                        chunk['data'].append(data_dict)
                if chunk['type'] == 'CAMR':
                    chunk['cameras'] = []
                    while len(data) > 0:
                        data, data_dict = read_dict_from_data(data)
                        key = list(data_dict.keys())[0]
                        if key in ['dist']:
                            data_dict[key] = unpack('f', data_dict[key])[0]
                        if key in ['name']:
                            data_dict[key] = data_dict[key].decode()
                        if key in ['active']:
                            data_dict[key] = 1
                        if key in ['rot', 'ofs']:
                            array_value = []
                            while len(data_dict[key]) > 0:
                                array_value.append(unpack('f', data_dict[key][:4])[0])
                                data_dict[key] = data_dict[key][4:]
                            data_dict[key] = array_value
                        chunk['cameras'].append(data_dict)
                file_dict['chunks'].append(chunk)
    except FileNotFoundError as e:
        print('ERROR: File "{}" not found.'.format(filename))
        exit(1)
    
    import pprint
    pprint.pprint(file_dict)

if __name__ == "__main__":
    main()
