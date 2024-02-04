project(
  'Megaminx',
  ['c','cpp'],
  version : '1.4.4',
  default_options : ['warning_level=3', 'cpp_std=c++20'],
  license: 'AGPL3'
)

freeglut_dep = dependency('freeglut')
opengl_dep = dependency('OpenGL')
glu_dep = dependency('glu')

incdir = include_directories('src')

MEGAMINX_sources = files(
    'src/ui/linenoise.c',
    'src/readline.cpp',
    'src/ui/camera.cpp',
    'src/ui/opengl.cpp',
    'src/engine/center.cpp',
    'src/engine/corner.cpp',
    'src/engine/edge.cpp',
    'src/engine/face.cpp',
    'src/engine/megaminx.cpp',
    'src/engine/solve.cpp',
    'src/engine/shadow.cpp',
    'src/engine/load.cpp',
    'src/main-menu.cpp',
    'src/main.cpp',
#    'src/Res.rc',
    'src/engine/algo-menu.hpp',
)

#Sources - Add .H Files 
MEGAMINX_headers = files(
    'src/ui/linenoise.h',  
    'src/ui/camera.hpp',
    'src/ui/opengl.h',
    'src/engine/algorithms.hpp',
    'src/engine/piece.hpp',
    'src/engine/piece-color.hpp',
    'src/engine/piece-static.hpp',
    'src/engine/center.hpp',
    'src/engine/corner.hpp',
    'src/engine/edge.hpp',
    'src/engine/face.hpp',
    'src/engine/megaminx.hpp',
    'src/engine/load.hpp',
    'src/main-menu.hpp',
    'src/main.h',
)

executable('megaminx',
 MEGAMINX_sources,
 dependencies : [freeglut_dep, opengl_dep, glu_dep],
 include_directories : incdir,
 install : true
)

