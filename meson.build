project(
  'Megaminx',
  ['c', 'cpp'],
  version: '1.4.6',
  default_options: ['warning_level=3', 'cpp_std=c++20'],
  license: 'AGPL3',
)

opengl_dep = dependency('OpenGL')
glu_dep = dependency('glu')
freeglut_dep = dependency('freeglut')

incdir = include_directories('src')

MEGAMINX_sources = files(
  'src/engine/center.cpp',
  'src/engine/corner.cpp',
  'src/engine/edge.cpp',
  'src/engine/face.cpp',
  'src/engine/load.cpp',
  'src/engine/megaminx.cpp',
  'src/engine/shadow.cpp',
  'src/engine/solve.cpp',
  'src/main-menu.cpp',
  'src/main.cpp',
  #  'src/Res.rc',
  'src/readline.cpp',
  'src/ui/camera.cpp',
  'src/ui/linenoise.c',
  'src/ui/opengl.cpp',
  'src/ui/mouse-ray.cpp',
)

MEGAMINX_headers = files(
  'src/engine/algo-menu.hpp',
  'src/engine/algorithms.hpp',
  'src/engine/center.hpp',
  'src/engine/corner.hpp',
  'src/engine/edge.hpp',
  'src/engine/face.hpp',
  'src/engine/load.hpp',
  'src/engine/megaminx.hpp',
  'src/engine/piece-color.hpp',
  'src/engine/piece-static.hpp',
  'src/engine/piece.hpp',
  'src/main-menu.hpp',
  'src/main.h',
  'src/ui/camera.hpp',
  'src/ui/linenoise.h',
  'src/ui/opengl.h',
  'src/ui/mouse-ray.hpp',
)

executable(
  'megaminx',
  MEGAMINX_sources,
  dependencies: [freeglut_dep, opengl_dep, glu_dep],
  include_directories: incdir,
  install: true,
)
