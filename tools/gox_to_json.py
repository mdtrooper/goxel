#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import json
import struct

from goxel import Goxel

# https://pypi.org/project/pypng/
# pip install pypng
import png

gox_clean = {}

def main(filename, store_img = False):
    goxfile = Goxel.from_file(filename)
    
    gox_clean['header'] = {'version': goxfile.header.version}
    bl16s = []
    layers = []
    cameras = []
    materials = []
    
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
        
        if chunk.type == 'IMG ':
            img = dict()
            data_img = chunk.data.img
            if data_img.value_size == 0:
                img[data_img.key] = False
            else:                   
                if type(data_img.value) == bytes:
                    img[data_img.key] = ''.join(['{:02x}'.format(byte) for byte in data_img.value])
                else:
                    img[data_img.key] = data_img.value
            gox_clean['image'] = img
        
        if chunk.type == 'MATE':
            data_material = chunk.data
            material = dict()
            for data in data_material.mate:
                if data.value_size == 0:
                    material[data.key] = False
                else:
                    if data.key == 'color':
                        material[data.key] = [data.value.r, data.value.g, data.value.b, data.value.a] 
                    else:
                        if type(data.value) == bytes:
                            material[data.key] = ''.join(['{:02x}'.format(byte) for byte in data.value])
                        else:
                            material[data.key] = data.value
            materials.append(material)
        
        if chunk.type == 'CAMR':
            data_camera = chunk.data
            camera = dict()
            for data in data_camera.camera:
                if data.value_size == 0:
                    camera[data.key] = False
                else:
                    if data.key == 'ortho':
                        camera[data.key], *_ = struct.unpack('<?', data.value)
                    else:
                        if type(data.value) == bytes:
                            camera[data.key] = ''.join(['{:02x}'.format(byte) for byte in data.value])
                        else:
                            camera[data.key] = data.value
            cameras.append(camera)
        
        if chunk.type == 'LAYR':
            data_layer = chunk.data
            layer = dict()
            voxels = dict()
            for data in data_layer.dict:
                if data.key == 'visible':
                    layer[data.key], *_ = struct.unpack('<?', data.value)
                else:
                    if type(data.value) == bytes:
                        layer[data.key] = ''.join(['{:02x}'.format(byte) for byte in data.value])
                    else:
                        layer[data.key] = data.value
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
                            voxels[vx][vy][vz] = '{:02x}{:02x}{:02x}'.format(*color[:3])
                        y += 1
                    x += 1
            layer['voxels'] = voxels
            
            layers.append(layer)
    
    gox_clean['layers'] = layers
    gox_clean['cameras'] = cameras
    gox_clean['materials'] = materials
    
    print(json.dumps(gox_clean, indent=4))

if __name__ == "__main__":
    filename = sys.argv[1]
    store_img = False
    if 2 in sys.argv:
        if sys.argv[2] == '--store-img':
            store_img = True
    main(filename, store_img)
