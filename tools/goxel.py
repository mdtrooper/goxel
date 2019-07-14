# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
from kaitaistruct import __version__ as ks_version, KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum


if parse_version(ks_version) < parse_version('0.7'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.7 or later is required, but you have %s" % (ks_version))

class Goxel(KaitaiStruct):
    """Goxel is a 3D editor for voxel scenes.
    
    .. seealso::
       Source - https://github.com/guillaumechereau/goxel/blob/master/src/formats/gox.c
    """

    class ColorTypes(Enum):
        greyscale = 0
        truecolor = 2
        indexed = 3
        greyscale_alpha = 4
        truecolor_alpha = 6

    class CompressionMethods(Enum):
        zlib = 0
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._read()

    def _read(self):
        self.header = self._root.Header(self._io, self, self._root)
        self.chunk = []
        i = 0
        while not self._io.is_eof():
            self.chunk.append(self._root.Chunk(self._io, self, self._root))
            i += 1


    class Header(KaitaiStruct):
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._read()

        def _read(self):
            self.magic = self._io.ensure_fixed_contents(b"\x47\x4F\x58\x20")
            self.version = self._io.read_u4le()


    class Chunk(KaitaiStruct):
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._read()

        def _read(self):
            self.type = (self._io.read_bytes(4)).decode(u"ASCII")
            self.size = self._io.read_u4le()
            _on = self.type
            if _on == u"BL16":
                self._raw_data = self._io.read_bytes(self.size)
                _io__raw_data = KaitaiStream(BytesIO(self._raw_data))
                self.data = self._root.Chunk.Bl16(_io__raw_data, self, self._root)
            elif _on == u"MATE":
                self._raw_data = self._io.read_bytes(self.size)
                _io__raw_data = KaitaiStream(BytesIO(self._raw_data))
                self.data = self._root.Chunk.Mate(_io__raw_data, self, self._root)
            elif _on == u"CAMR":
                self._raw_data = self._io.read_bytes(self.size)
                _io__raw_data = KaitaiStream(BytesIO(self._raw_data))
                self.data = self._root.Chunk.Camera(_io__raw_data, self, self._root)
            elif _on == u"LAYR":
                self._raw_data = self._io.read_bytes(self.size)
                _io__raw_data = KaitaiStream(BytesIO(self._raw_data))
                self.data = self._root.Chunk.Layer(_io__raw_data, self, self._root)
            elif _on == u"IMG ":
                self._raw_data = self._io.read_bytes(self.size)
                _io__raw_data = KaitaiStream(BytesIO(self._raw_data))
                self.data = self._root.Chunk.Img(_io__raw_data, self, self._root)
            else:
                self.data = self._io.read_bytes(self.size)
            self.crc = self._io.read_u4le()

        class Img(KaitaiStruct):
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._read()

            def _read(self):
                self._raw_img = self._io.read_bytes(self._parent.size)
                _io__raw_img = KaitaiStream(BytesIO(self._raw_img))
                self.img = self._root.Chunk.Dict(_io__raw_img, self, self._root)


        class Camera(KaitaiStruct):
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._read()

            def _read(self):
                self.camera = []
                i = 0
                while not self._io.is_eof():
                    self.camera.append(self._root.Chunk.Dict(self._io, self, self._root))
                    i += 1



        class Bl16(KaitaiStruct):
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._read()

            def _read(self):
                self.magic = self._io.ensure_fixed_contents(b"\x89\x50\x4E\x47\x0D\x0A\x1A\x0A")
                self.png = []
                i = 0
                while not self._io.is_eof():
                    self.png.append(self._root.Chunk.Bl16.ChunkPng(self._io, self, self._root))
                    i += 1


            class ChunkPng(KaitaiStruct):
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._read()

                def _read(self):
                    self.chunk_png_size = self._io.read_u4be()
                    self.chunk_png_type = (self._io.read_bytes(4)).decode(u"ASCII")
                    _on = self.chunk_png_type
                    if _on == u"IHDR":
                        self._raw_chunk_png_data = self._io.read_bytes(self.chunk_png_size)
                        _io__raw_chunk_png_data = KaitaiStream(BytesIO(self._raw_chunk_png_data))
                        self.chunk_png_data = self._root.Chunk.Bl16.ChunkPng.Ihdr(_io__raw_chunk_png_data, self, self._root)
                    else:
                        self.chunk_png_data = self._io.read_bytes(self.chunk_png_size)
                    self.chunk_png_crc = self._io.read_bytes(4)

                class Ihdr(KaitaiStruct):
                    def __init__(self, _io, _parent=None, _root=None):
                        self._io = _io
                        self._parent = _parent
                        self._root = _root if _root else self
                        self._read()

                    def _read(self):
                        self.width = self._io.read_u4be()
                        self.height = self._io.read_u4be()
                        self.bit_depth = self._io.read_u1()
                        self.color_type = KaitaiStream.resolve_enum(self._root.ColorTypes, self._io.read_u1())
                        self.compression_method = KaitaiStream.resolve_enum(self._root.CompressionMethods, self._io.read_u1())
                        self.filter_method = self._io.read_u1()
                        self.interlace_method = self._io.read_u1()




        class Layer(KaitaiStruct):
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._read()

            def _read(self):
                self.count_blocks = self._io.read_u4le()
                self.block = [None] * (self.count_blocks)
                for i in range(self.count_blocks):
                    self.block[i] = self._root.Chunk.Layer.Block(self._io, self, self._root)

                self.dict = []
                i = 0
                while not self._io.is_eof():
                    self.dict.append(self._root.Chunk.Dict(self._io, self, self._root))
                    i += 1


            class Block(KaitaiStruct):
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._read()

                def _read(self):
                    self.index = self._io.read_s4le()
                    self.x = self._io.read_s4le()
                    self.y = self._io.read_s4le()
                    self.z = self._io.read_u4le()
                    self.zero = self._io.read_u4le()



        class Mate(KaitaiStruct):
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._read()

            def _read(self):
                self.mate = []
                i = 0
                while not self._io.is_eof():
                    self.mate.append(self._root.Chunk.Dict(self._io, self, self._root))
                    i += 1



        class Dict(KaitaiStruct):
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._read()

            def _read(self):
                self.key_size = self._io.read_u4le()
                self.key = (self._io.read_bytes(self.key_size)).decode(u"ASCII")
                self.value_size = self._io.read_u4le()
                _on = self.key
                if _on == u"dist":
                    self.value = self._io.read_f4le()
                elif _on == u"base_id":
                    self.value = self._io.read_u4le()
                elif _on == u"material":
                    self.value = self._io.read_u4le()
                elif _on == u"metallic":
                    self.value = self._io.read_f4le()
                elif _on == u"id":
                    self.value = self._io.read_u4le()
                elif _on == u"color":
                    self._raw_value = self._io.read_bytes(self.value_size)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = self._root.Chunk.Dict.Color(_io__raw_value, self, self._root)
                elif _on == u"roughness":
                    self.value = self._io.read_f4le()
                elif _on == u"name":
                    self.value = (self._io.read_bytes(self.value_size)).decode(u"ASCII")
                elif _on == u"emission":
                    self.value = self._io.read_f4le()
                else:
                    self.value = self._io.read_bytes(self.value_size)

            class Color(KaitaiStruct):
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._read()

                def _read(self):
                    self.r = self._io.read_f4le()
                    self.g = self._io.read_f4le()
                    self.b = self._io.read_f4le()
                    self.a = self._io.read_f4le()





