#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import json

from goxel import Goxel

# https://pypi.org/project/pypng/
# pip install pypng
import png

# For debug
g = None
img = None

gox_clean = {}

def main(filename):
    global g
    global img
    
    # ~ print(filename)
    
    g = Goxel.from_file(filename)
    
    gox_clean['header'] = {'version': g.header.version}
    bl16s = []
    layers = []
    
    # ~ index_block = 0
    
    for chunk in g.chunk:
        if chunk.chunk_type == 'BL16':
            # ~ bl16s.append(chunk._raw_data)
            
            img_png = png.Reader(bytes=chunk._raw_data)
            img = img_png.asRGBA8()
            
            rows = list(img[2])
            
            bl16 = [None] * len(rows)
            
            x = 0
            for row in rows:
                bl16[x] = [None] * int(len(row) / 4)
                y = 0
                for r, g, b, a in zip(*[iter(row)]*4):
                    bl16[x][y] = (r,g,b,a)
                    y += 1
                x += 1
            bl16s.append(bl16)
            # ~ print(img)
            
            
            # ~ aaa = png.Reader('/tmp/caca.png')
            # ~ bbb = aaa.asRGBA8()
            # ~ l = list(bbb[2])
            # ~ x = 0
            # ~ for row in l:
                # ~ y = 0
                # ~ for r, g, b, a in zip(*[iter(row)]*4):
                    # ~ print("({x: >2},{y: >2}) ({r: >3},{g: >3},{b: >3},{a: >3})".format(x=x,y=y,r=r,g=g,b=b,a=a))
                    # ~ y += 1
                # ~ x += 1
        if chunk.chunk_type == 'LAYR':
            data_layer = chunk.data
            layer = {'content': []}
            # ~ for it_block in range(data_layer.count_blocks):
                # ~ layer['blocks'].append(bl16s[index_block])
                # ~ index_block += 1
            
            layers.append(layer)
    
    gox_clean['layers'] = layers
    
    print(json.dumps(gox_clean))

if __name__ == "__main__":
    filename = sys.argv[1]
    main(filename)
