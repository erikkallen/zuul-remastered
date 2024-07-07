project('zuul', 'c')

fs = import('fs')

base_dir = meson.current_source_dir()
incdir = include_directories('include', 'lib/log.c/src', 'lib/hshg/c', 'lib/hashmap.c')

sdl3_dep = dependency('sdl3')
sdl3_image_dep = dependency('SDL3_image')
#sdl3_ttf_dep = dependency('SDL3_ttf')
cjson_dep = dependency('libcjson')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: true)
valgrind = find_program('valgrind', required: false)

deps = [sdl3_dep, sdl3_image_dep, cjson_dep, m_dep]

sources = files(
  'lib/hashmap.c/hashmap.c',
  'lib/hshg/c/hshg.c',
  'lib/log.c/src/log.c',
  'src/assets.c',
  'src/draw.c',
  'src/init.c',
  'src/input.c',
  'src/main.c',
  'src/map.c',
  'src/network.c',
  'src/player.c',
  'src/tileset.c',
)
server_sources = files('lib/log.c/src/log.c', 'src/server.c')

executable(
  'zuul',
  sources,
  include_directories: incdir,
  dependencies: deps,
  c_args: ['-DLOG_USE_COLOR', '-DHSHG_D=2', '-DHSHG_UNIFORM'],
)
executable(
  'zuul_server',
  server_sources,
  include_directories: incdir,
  c_args: ['-DLOG_USE_COLOR'],
)

test_sources = files(
  'lib/log.c/src/log.c',
  'src/assets.c',
  'src/map.c',
  'src/tileset.c',
  'tests/map_test.c',
)
map_test = executable(
  'map_test',
  test_sources,
  include_directories: incdir,
  dependencies: deps,
  c_args: ['-DLOG_USE_COLOR', '-DHSHG_D=2', '-DHSHG_UNIFORM'],
)
if valgrind.found()
  test(
    'map memory test',
    valgrind,
    args: ['--leak-check=full', '--error-exitcode=1', map_test.full_path()],
  )
else
  message('Valgrind not found: skipping memory leak tests.')
endif
