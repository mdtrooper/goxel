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
    size, *_ = unpack('<i', data[:4])
    data = data[4:]
    
    key = data[:size].decode()
    data = data[size:]
    
    size, *_ = unpack('<i', data[:4])
    data = data[4:]
    
    value = data[:size]
    data = data[size:]
    
    return data, {key: value}

def gox_to_json(filename):
    index = 0
    
    file_dict = {}
    file_dict['goxel_version'] = '0.7.3'
    try:
        with open(filename, 'rb') as f:
            file_dict['magic_string'] = f.read(4).decode()
            
            if file_dict['magic_string'] != 'GOX ':
                print('ERROR: The file is not a gox file.')
                exit(1)
            
            file_dict['version'] , *_ = unpack('<i', f.read(4))
            
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
                size, *_ = unpack('<i', f.read(4))
                data = f.read(size)
                crc, *_ = unpack('<i', f.read(4))
                
                if chunk['type'] == 'IMG':
                    while len(data) > 0:
                        data, data_dict = read_dict_from_data(data)
                        key = list(data_dict.keys())[0]
                        chunk[key] = []
                        # ~ with open('test.png', 'wb') as img:
                            # ~ img.write(data_dict[key])
                        # ~ print("{} {} {} {}".format('data_dict[key]', len(data_dict[key]), type(data_dict[key]), data_dict[key]))
                        for i in range(0, 4):
                            chunk[key].append(unpack('ffff', data_dict[key][:4*4]))
                            data_dict[key] = data_dict[key][4*4:]
                if chunk['type'] == 'PREV':
                    chunk['data'] = data
                if chunk['type'] == 'BL16':
                    print("")
                    print("{} {}".format(len(data), data))
                    # PNG Header
                    eightynine, *_ = unpack('c', data[:1])
                    data = data[1:]
                    png, *_ = unpack('3s', data[:3])
                    data = data[3:]
                    _ = unpack('4c', data[:4])
                    data = data[4:]
                    
                    while len(data) > 0:
                        size_png_chunk, *_ = unpack('>i', data[:4])
                        print("{}".format(size_png_chunk))
                        data = data[4:]
                        type_png_chunk = data[:4].decode().strip()
                        data = data[4:]
                        data_png_chunk = data[:size_png_chunk]
                        data = data[size_png_chunk:]
                        crc_png_chunk, *_ = unpack('<I', data[:4])
                        data = data[4:]
                        if type_png_chunk == 'IHDR':
                            width_png, *_ = unpack('>i', data_png_chunk[:4])
                            data_png_chunk = data_png_chunk[4:]
                            height_png, *_ = unpack('>i', data_png_chunk[:4])
                            data_png_chunk = data_png_chunk[4:]
                            bit_pixel_png, *_ = unpack('b', data_png_chunk[:1])
                        if type_png_chunk == 'IDAT':
                            chunk['data'] = data_png_chunk
                            chunk['index'] = index
                            index += 1
                if chunk['type'] == 'LAYR':
                    num_blocks, *_ = unpack('<i', data[:4])
                    data = data[4:]
                    
                    chunk['blocks'] = []
                    count_blocks = 0
                    while count_blocks < num_blocks:
                        index_BL16, *_ = unpack('<i', data[:4])
                        data = data[4:]
                        
                        x, y, z = unpack('<iii', data[:4*3])
                        data = data[4*3:]
                        
                        data = data[4:] # Move a int32 ahead
                        
                        # TODO: Get the v from BL16
                        
                        chunk['blocks'].append({'index': index_BL16, 'x': x, 'y': y, 'z': z, 'v': None})
                        count_blocks += 1
                    
                    chunk['data'] = []
                    while len(data) > 0:
                        data, data_dict = read_dict_from_data(data)
                        key = list(data_dict.keys())[0]
                        if key in ['id', 'base_id']:
                            data_dict[key] , *_ = unpack('<i', data_dict[key])
                        if key in ['name']:
                            data_dict[key] = data_dict[key].decode()
                        if key in ['mat']:
                            data_mat = data_dict[key]
                            data_dict[key] = []
                            for i in range(0, 4):
                                data_dict[key].append(unpack('ffff', data_mat[:4*4]))
                                data_mat = data_mat[4*4:]
                        chunk['data'].append(data_dict)
                if chunk['type'] == 'CAMR':
                    chunk['active'] = 0
                    while len(data) > 0:
                        data, data_dict = read_dict_from_data(data)
                        key = list(data_dict.keys())[0]
                        if key in ['dist']:
                            chunk[key] = unpack('f', data_dict[key])[0]
                        if key in ['name']:
                            chunk[key] = data_dict[key].decode()
                        if key in ['active']:
                            chunk[key] = 1
                        if key in ['rot']:
                            chunk[key] = unpack('ffff', data_dict[key][:4*4])
                        if key in ['ofs']:
                            chunk[key] = unpack('fff', data_dict[key][:4*3])
                file_dict['chunks'].append(chunk)
    except FileNotFoundError as e:
        print('ERROR: File "{}" not found.'.format(filename))
        exit(1)
    
    import pprint
    pprint.pprint(file_dict)

if __name__ == "__main__":
    main()
