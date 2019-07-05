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
      - id: data_size
        type: u4
      - id: data
        size: data_size
        type:
          switch-on: chunk_type
          cases:
            '"IMG "': img
            '"LAYR"': layer
      - id: crc
        type: u4
    types:
      img:
        seq:
         - id: data_img
           type: dict
           size: _parent.data_size
      layer:
        seq:
          - id: count_blocks
            type: u4
          - id: block
            type: block
            repeat: expr
            repeat-expr: count_blocks
          - id: block_dict
            type: dict
            repeat: eos
        types:
          block:
            seq:
              - id: block_index
                type: u4
              - id: x
                type: u4
              - id: y
                type: u4
              - id: z
                type: u4
              - id: zero
                type: u4
      dict:
        seq:
          - id: key_size
            type: u4
          - id: key
            type: str
            size: key_size
          - id: value_size
            type: u4
          - id: value
            size: value_size
