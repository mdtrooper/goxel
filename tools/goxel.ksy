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
