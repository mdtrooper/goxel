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
  - id: bl1z
    type: bl16
  - id: bl1zz
    type: bl16
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
      - id: version
        type: u4
  img:
    seq:
      - id: magic
        contents: 'IMG '
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
      - id: length
        type: u4
      - id: unknown
        size: length
      - id: crc
        type: u4
  camera:
    seq:
      - id: magic
        contents: 'CAMR'
      - id: length
        type: u4
      - id: unknown
        size: length
      - id: crc
        type: u4
instances:
  value_size:
    value: img.length - 4 - img.dict.length_key
