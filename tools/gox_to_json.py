#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import json

from goxel import Goxel

# https://pypi.org/project/pypng/
# pip install pypng
import png

# For debug
goxfile = None
img = None

gox_clean = {}

# ~ In [15]: x = 0  
    # ~ ...: for col in bl16s[0]:  
    # ~ ...:     y = 0  
    # ~ ...:     for color in col:  
    # ~ ...:         if color[3] != 0: 
    # ~ ...:             pos = x + (y * 64) 
    # ~ ...:             vz = int(pos / 16**2) 
    # ~ ...:             res = pos % 16**2 
    # ~ ...:             vy = int(res / 16) + 16 # offset 16 from layer 0,16,0 
    # ~ ...:             vx = res % 16 
    # ~ ...:             print("{} {} {} - ".format(vx, vy, vz), end="") 
    # ~ ...:             print("({x: >2},{y: >2}) ({r: >3},{g: >3},{b: >3},{a: >3})".format(x=x,y=y,r=color[0],g=color[1],b=color[2],a=color[3])) 
    # ~ ...:         y += 1  
    # ~ ...:     x += 1 
    # ~ ...:                                                                                                                                                                
# ~ 0 28 0 - ( 0, 3) (  0,  0,  0,255)
# ~ 4 28 0 - ( 4, 3) (  0,255,  0,255)
# ~ 0 29 0 - (16, 3) (  0,  0,  0,255)
# ~ 4 29 0 - (20, 3) (  0,255,  0,255)
# ~ 0 30 0 - (32, 3) (  0,  0,  0,255)
# ~ 4 30 0 - (36, 3) (  0,255,  0,255)
# ~ 0 27 0 - (48, 2) (  0,  0,  0,255)
# ~ 0 31 0 - (48, 3) (255,255,255,255)
# ~ 0 31 1 - (48, 7) (255,  0,  0,255)
# ~ 0 31 2 - (48,11) (255,  0,  0,255)
# ~ 0 31 3 - (48,15) (255,  0,  0,255)
# ~ 0 31 4 - (48,19) (255,  0,  0,255)
# ~ 1 27 0 - (49, 2) (  0,  0,255,255)
# ~ 1 31 0 - (49, 3) (255,255,255,255)
# ~ 2 27 0 - (50, 2) (  0,  0,255,255)
# ~ 2 31 0 - (50, 3) (255,255,255,255)
# ~ 3 27 0 - (51, 2) (  0,  0,255,255)
# ~ 3 31 0 - (51, 3) (255,255,255,255)
# ~ 4 27 0 - (52, 2) (  0,  0,255,255)
# ~ 4 31 0 - (52, 3) (  0,255,  0,255)




def main(filename, store_img = False):
    global goxfile
    global img
    global bl16s
    
    print(filename)
    
    goxfile = Goxel.from_file(filename)
    
    gox_clean['header'] = {'version': goxfile.header.version}
    bl16s = []
    layers = []
    
    index_block = 0
    for chunk in goxfile.chunk:
        if chunk.type == 'BL16':
            # ~ bl16s.append(chunk._raw_data)
            if store_img:
                with open('block{}.png'.format(index_block), 'wb') as f:
                    f.write(chunk._raw_data)
            
            
            # ~ aaa = png.Reader('/tmp/caca.png')
            img_png = png.Reader(bytes=chunk._raw_data)
            img = img_png.asRGBA8()
            
            rows = list(img[2])
            
            bl16 = [[None for x in range(img[3]['size'][0])] for y in range(img[3]['size'][1])]
            
            y = 0
            for row in rows:
                x = 0
                for r, g, b, a in zip(*[iter(row)]*4):
                    bl16[x][y] = (r,g,b,a)
                    # ~ print("({x: >2},{y: >2}) ({r: >3},{g: >3},{b: >3},{a: >3})".format(x=x,y=y,r=r,g=g,b=b,a=a))
                    x += 1
                y += 1
            bl16s.append(bl16)
            
            index_block += 1
            
        if chunk.type == 'LAYR':
            data_layer = chunk.data
            layer = dict()
            voxels = dict()
            for block in data_layer.block:
                x = 0
                for col in bl16s[block.index]:
                    y = 0  
                    for color in col:
                        if color[3] != 0: # Not transparent
                            pos = x + (y * 64)
                            vz = int(pos / 16**2) + block.z # offset block z
                            res = pos % 16**2 
                            vy = int(res / 16) + block.y # offset block y
                            vx = (res % 16) + block.x # offset block x
                            
                            if not(vx in voxels):
                                voxels[vx] = dict()
                            if not(vy in voxels):
                                voxels[vx][vy] = dict()
                            if not(vz in voxels):
                                voxels[vx][vy][vz] = None
                            voxels[vx][vy][vz] = color[:3]
                        y += 1
                    x += 1
            layer['voxels'] = voxels
            
            layers.append(layer)
    
    gox_clean['layers'] = layers
    
    print(json.dumps(gox_clean))

if __name__ == "__main__":
    filename = sys.argv[1]
    store_img = False
    if 2 in sys.argv:
        if sys.argv[2] == '--store-img':
            store_img = True
    main(filename, store_img)
