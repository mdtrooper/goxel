#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Translate a gox binary file to json file.

__author__ = "Miguel"
__copyright__ = "Copyright 2018"
__license__ = "GPL"

from sys import argv
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
    try:
        with open(filename, 'rb') as f:
            magic_string = f.read(4).decode()
            
            if magic_string.decode() != 'GOX ':
                print('ERROR: The file is not a gox file.')
                exit(1)
            
            version = f.read(4)[0]
            
            
            #~ for b in iter(lambda: f.read(1), b''):
                #~ print(b, end='')
    except FileNotFoundError as e:
        print('ERROR: File "{}" not found.'.format(filename)
        exit(1)

if __name__ == "__main__":
    main()
