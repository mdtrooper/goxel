meta:
  id: goxel
  file-extension: gox
  license: GPL3
  title: Goxel default store file
  endian: le

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
      - id: data
        size: length
      - id: crc
        type: u4A
  bl16:
    seq:
      - id: magic
        contents: 'BL16'
