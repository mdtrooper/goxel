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
      - id: size
        type: u4
      - id: data
        size: size
        type:
          switch-on: type
          cases:
            '"IMG "': img
            '"LAYR"': layer
            '"BL16"': bl16
            '"MATE"': mate
            '"CAMR"': camera
      - id: crc
        type: u4
    types:
      img:
        seq:
         - id: img
           type: dict
           size: _parent.size
      layer:
        seq:
          - id: count_blocks
            type: u4
          - id: block
            type: block
            repeat: expr
            repeat-expr: count_blocks
          - id: dict
            type: dict
            repeat: eos
        types:
          block:
            seq:
              - id: index
                type: s4
              - id: x
                type: s4
              - id: y
                type: s4
              - id: z
                type: u4
              - id: zero
                type: u4
      bl16:
        seq:
          - id: magic
            contents: [137, 80, 78, 71, 13, 10, 26, 10]
          - id: png
            type: chunk_png
            repeat: eos
        types:
          chunk_png:
            seq:
              - id: chunk_png_size
                type: u4be
              - id: chunk_png_type
                type: str
                size: 4
              - id: chunk_png_data
                size: chunk_png_size
                type:
                  switch-on: chunk_png_type
                  cases:
                    '"IHDR"': ihdr
              - id: chunk_png_crc
                size: 4
            types:
              ihdr:
                seq:
                  - id: width
                    type: u4be
                  - id: height
                    type: u4be
                  - id: bit_depth
                    type: u1
                  - id: color_type
                    type: u1
                    enum: color_types
                  - id: compression_method
                    type: u1
                    enum: compression_methods
                  - id: filter_method
                    type: u1
                  - id: interlace_method
                    type: u1
      mate:
        seq:
          - id: mate
            type: dict
            repeat: eos
      camera:
        seq:
          - id: camera
            type: dict
            repeat: eos
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
            type:
              switch-on: key
              cases:
                '"name"': str
                '"dist"': f4
                #'"ortho"': b1
                #'"active"': b1
                #'"visible"': b1
                '"metallic"': f4
                '"roughness"': f4
                '"emission"': f4
                '"color"': color
                '"material"': u4
                '"id"': u4
                '"base_id"': u4
                '"material"': u4
        types:
          color:
            seq:
              - id: r
                type: f4
              - id: g
                type: f4
              - id: b
                type: f4
              - id: a
                type: f4
enums:
  color_types:
    0: greyscale
    2: truecolor
    3: indexed
    4: greyscale_alpha
    6: truecolor_alpha
  compression_methods:
    0: zlib
