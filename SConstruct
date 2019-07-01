# Goxel 3D voxels editor
#
# copyright (c) 2018 Guillaume Chereau <guillaume@noctua-software.com>
#
# Goxel is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# goxel.  If not, see <http://www.gnu.org/licenses/>.

import glob
import os
import sys

vars = Variables('settings.py')
vars.AddVariables(
    EnumVariable('mode', 'Build mode', 'debug',
        allowed_values=('debug', 'release', 'profile')),
    BoolVariable('werror', 'Warnings as error', True),
    BoolVariable('sound', 'Enable sound', False),
    PathVariable('config_file', 'Config file to use', 'src/config.h'),
)

target_os = str(Platform())

env = Environment(variables = vars, ENV = os.environ)
conf = env.Configure()

if os.environ.get('CC') == 'clang':
    env.Replace(CC='clang', CXX='clang++')

# Asan & Ubsan (need to come first).
if env['mode'] == 'debug' and target_os == 'posix':
    env.Append(CCFLAGS=['-fsanitize=address', '-fsanitize=undefined'],
               LINKFLAGS=['-fsanitize=address', '-fsanitize=undefined'],
               LIBS=['asan', 'ubsan'])


# Global compilation flags.
# CCFLAGS   : C and C++
# CFLAGS    : only C
# CXXFLAGS  : only C++
env.Append(
    CFLAGS=['-std=gnu99', '-Wall',
            '-Wno-unknow-pragma', '-Wno-unknown-warning-option'],
    CXXFLAGS=['-std=gnu++17', '-Wall', '-Wno-narrowing']
)

if env['werror']:
    env.Append(CCFLAGS='-Werror')


if env['mode'] == 'debug':
    env.Append(CCFLAGS=['-O0'])
else:
    env.Append(CCFLAGS='-O3', CPPDEFINES='NDEBUG')
    if env['CC'] == 'gcc': env.Append(CCFLAGS='-Ofast')

if env['mode'] in ('profile', 'debug'):
    env.Append(CCFLAGS='-g')

env.Append(CPPPATH=['src'])
env.Append(CCFLAGS=['-include', '$config_file'])

# Get all the c and c++ files in src, recursively.
sources = []
for root, dirnames, filenames in os.walk('src'):
    for filename in filenames:
        if filename.endswith('.c') or filename.endswith('.cpp'):
            sources.append(os.path.join(root, filename))

# Check for libpng.
if conf.CheckLibWithHeader('libpng', 'png.h', 'c'):
    env.Append(CPPDEFINES='HAVE_LIBPNG=1')

# Linux compilation support.
if target_os == 'posix':
    env.Append(LIBS=['GL', 'm', 'z'])
    # Note: add '--static' to link with all the libs needed by glfw3.
    env.ParseConfig('pkg-config --libs glfw3')
    env.ParseConfig('pkg-config --cflags --libs gtk+-3.0')

# Windows compilation support.
if target_os == 'msys':
    env.Append(CXXFLAGS=['-Wno-attributes', '-Wno-unused-variable',
                         '-Wno-unused-function'])
    env.Append(LIBS=['glfw3', 'opengl32', 'Imm32', 'gdi32', 'Comdlg32',
                     'z', 'tre', 'intl', 'iconv'],
               LINKFLAGS='--static')
    sources += glob.glob('ext_src/glew/glew.c')
    env.Append(CPPPATH=['ext_src/glew'])
    env.Append(CPPDEFINES=['GLEW_STATIC', 'FREE_WINDOWS'])

# OSX Compilation support.
if target_os == 'darwin':
    sources += glob.glob('src/*.m')
    env.Append(FRAMEWORKS=['OpenGL', 'Cocoa'])
    env.Append(LIBS=['m', 'z', 'glfw3', 'objc'])

# Add external libs.
env.Append(CPPPATH=['ext_src/uthash'])
env.Append(CPPPATH=['ext_src/stb'])
env.Append(CPPPATH=['ext_src/noc'])

if env['sound']:
    env.Append(LIBS='openal')
    env.Append(CPPDEFINES='SOUND=1')

# Append external environment flags
env.Append(
    CFLAGS=os.environ.get("CFLAGS", "").split(),
    CXXFLAGS=os.environ.get("CXXFLAGS", "").split(),
    LINKFLAGS=os.environ.get("LDFLAGS", "").split()
)

env.Program(target='goxel', source=sources)
