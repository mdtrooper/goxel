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
  - id: chunk
    type: chunk
    repeat: eos

types:
  header:
    seq:
      - id: magic
        contents: 'GOX '
        size: 4
      - id: version
        type: u4
  chunk:
    seq:
      - id: type
        type: str
        size: 4
      - id: data_length
        type: u4
      - id: data
        size: data_length
        type:
        switch-on: type
        cases:
          'IMG ': img
          _: unknown
      - id: crc
        type: u4
  img:
    seq:
      - id: dict
        type: dict
        size: length
  unknown:
    seq:
      - id: unknown
