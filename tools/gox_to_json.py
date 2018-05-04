#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Translate a gox binary file to json file.

__author__ = "Miguel"
__copyright__ = "Copyright 2018"
__license__ = "GPL"

from sys import argv
from os import stat
from os.path import isfile

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

def gox_to_json(filename):
    file_dict = {}
    
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
                chunk['length'] = int.from_bytes(f.read(4), byteorder='little') 
                data = f.read(chunk['length'])
                chunk['crc'] = int.from_bytes(f.read(4), byteorder='little')
                
                if chunk['type'] == 'IMG':
                    pass
                if chunk['type'] == 'PREV':
                    pass
                if chunk['type'] == 'BL16':
                    pass
                if chunk['type'] == 'LAYR':
                    pass
                if chunk['type'] == 'CAMR':
                    pass
                
                print("Type: '{}'".format(chunk['type']))
                print("Chunk size: " + str(chunk['length']))
                print("Chunk data('{}'): '{}'".format(len(data), data))
                print("Chunk crc: " + str(chunk['crc']))
                file_dict['chunks'].append(chunk)
    except FileNotFoundError as e:
        print('ERROR: File "{}" not found.'.format(filename))
        exit(1)

if __name__ == "__main__":
    main()
