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
        file_dict['size'] = stat(filename).st_size
            
        with open(filename, 'rb') as f:
            file_dict['magic_string'] = f.read(4).decode()
            
            if file_dict['magic_string'] != 'GOX ':
                print('ERROR: The file is not a gox file.')
                exit(1)
            
            file_dict['version'] = f.read(4)[0]
            
            # READ CHUNKS
            reverse_count = file_dict['size'] - 4 -4 # size - magic_string (4bytes) - version (4bytes)
            
            file_dict['chunks'] = []
            while reverse_count > 0:
                chunk = {}
                chunk['length'] = f.read(4).decode()
                chunk['type'] = f.read(4).decode()
                
                file_dict['chunks'].append(chunk)
            
            #~ for b in iter(lambda: f.read(1), b''):
                #~ print(b, end='')
    except FileNotFoundError as e:
        print('ERROR: File "{}" not found.'.format(filename))
        exit(1)

if __name__ == "__main__":
    main()
