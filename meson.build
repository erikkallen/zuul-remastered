project('zuul', 'c')

fs = import('fs')

base_dir = meson.current_source_dir()
incdir = include_directories('include', 'lib/log.c/src', 'lib/hshg/c', 'lib/hashmap.c')

sdl3_dep = dependency('sdl3')
#sdl3_image_dep = dependency('SDL3_image')
#sdl3_ttf_dep = dependency('SDL3_ttf')
cjson_dep = dependency('libcjson')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)
valgrind = find_program('valgrind', required: false)

deps = [sdl3_dep, cjson_dep, m_dep]

sources = files('src/main.c', 'lib/log.c/src/log.c', 'src/network.c', 'src/draw.c', 'src/init.c', 'src/input.c', 'src/player.c', 'src/map.c', 'src/tileset.c', 'lib/hshg/c/hshg.c', 'lib/hashmap.c/hashmap.c', 'src/assets.c')
server_sources = files('src/server.c', 'lib/log.c/src/log.c')

executable('zuul', sources, include_directories: incdir, dependencies: deps, c_args : ['-DLOG_USE_COLOR', '-DHSHG_D=2', '-DHSHG_UNIFORM'])
executable('zuul_server', server_sources, include_directories: incdir, c_args : ['-DLOG_USE_COLOR'])

test_sources = files('tests/map_test.c', 'src/map.c', 'lib/log.c/src/log.c', 'src/tileset.c', 'src/assets.c')
map_test = executable('map_test', test_sources, include_directories: incdir, dependencies: deps, c_args: ['-DLOG_USE_COLOR', '-DHSHG_D=2', '-DHSHG_UNIFORM'])
if valgrind.found()
    test('map memory test', valgrind,
         args: ['--leak-check=full', '--error-exitcode=1', map_test.full_path()])
else
    message('Valgrind not found: skipping memory leak tests.')
endif
