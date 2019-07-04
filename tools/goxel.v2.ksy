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
      - id: chunk_type
        type: str
        size: 4
      - id: data_length
        type: u4
      - id: data
        size: data_length
        type:
          switch-on: chunk_type
          cases:
            #'"KETCHUP"': rec_type_1
              _: type_unknown
      - id: crc
        type: u4
    types:
      img:
        seq:
         - id: data_img
           size: 50
      type_unknown:
        seq:
         - id: data_unknown
           size: 50
