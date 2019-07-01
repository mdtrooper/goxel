#Changelog

## [0.9.1]

### Added
- 'Magic Wand' tool.  Allows to select adjacent voxels.
- Show mirror axis on image box.

### Fixed
- Fixed bug in KVX export.
- Fixed crash with undo when we change materials or cameras.


## [0.9.0] - 2019-06-04

This major release brings proper material support, and better pathtracing
rendering.  The code has changed a lot, so expect a few bugs!

### Added
- Layer materials: each layer can now have its own material.
- Transparent materials.
- Emission materials.
- Support for png palettes.
- Add new view settings.
- Allow to scale a layer (only by factors of two).

### Changed
- Marching cube rendering default to 'flat' colors.
- Layer visibility is saved.
- Materials now use metallic/roughness settings.

### Fixed
- Bug with retina display on OSX.


## [0.8.3] - 2017-03-30

Minor release.

### Added
- Shape layers, for non destructible shapes creation.
- New path tracer, based on yocto-gl.  Totally remove the old one based on
  cycles.
- Support for exporting to KVX.
- Support for build engine palettes.

### Changed
- New default material used: lower the specular reflection.
