meta:
  id: goxel
  file-extension: gox
  license: GPL3
  title: Goxel default store file
  endian: le
  encoding: ASCII
doc: |
  Goxel is a 3D editor for voxel scenes.

doc-ref: https://github.com/guillaumechereau/goxel/blob/master/src/formats/gox.c
  
seq:
  - id: header
    type: header
  - id: img
    type: img
  - id: bl16
    type: bl16
    repeat: expr
    repeat-expr: 3 
  - id: mate
    type: mate
  - id: layer
    type: layer
  - id: camera
    type: camera

types:
  header:
    seq:
      - id: magic
        contents: 'GOX '
        size: 4
      - id: version
        type: u4
  img:
    seq:
      - id: magic
        contents: 'IMG '
        size: 4
      - id: length
        type: u4
      - id: dict
        type: dict
        size: length
      - id: crc
        type: u4
  bl16:
    seq:
      - id: magic
        contents: 'BL16'
        size: 4
      - id: length
        type: u4
      - id: unknown
        size: length
      - id: crc
        type: u4
  dict:
    seq:
      - id: length_key
        type: u4
      - id: key
        size: length_key
        type: str
      - id: value
        size: _root.value_size
  mate:
    seq:
      - id: magic
        contents: 'MATE'
        size: 4
      - id: length
        type: u4
      - id: unknown
        size: length
      - id: crc
        type: u4
  layer:
    seq:
      - id: magic
        contents: 'LAYR'
        size: 4
      - id: length
        type: u4
      - id: layer_data
        size: length
        type: layer_data
      - id: crc
        type: u4
  layer_data:
    seq:
      - id: num_blocks
        type: u4
      - id: block_index
        type: u4
      - id: x
        type: u4
      - id: y
        type: u4
      - id: z
        type: u4
  camera:
    seq:
      - id: magic
        contents: 'CAMR'
        size: 4
      - id: length
        type: u4
      - id: unknown
        size: length
      - id: crc
        type: u4
instances:
  value_size:
    value: img.length - 4 - img.dict.length_key
  
