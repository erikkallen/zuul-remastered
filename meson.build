project('zuul', 'c')

fs = import('fs')

base_dir = meson.current_source_dir()
incdir = include_directories('include', 'lib/log.c/src', 'lib/hshg/c')

sdl2_dep = dependency('sdl2')
sdl2_image_dep = dependency('SDL2_image')
sdl2_ttf_dep = dependency('SDL2_ttf')
cjson_dep = dependency('libcjson')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)
valgrind = find_program('valgrind', required: false)

deps = [sdl2_dep, sdl2_image_dep, sdl2_ttf_dep, cjson_dep, m_dep]

sources = files('src/main.c', 'lib/log.c/src/log.c', 'src/draw.c', 'src/init.c', 'src/input.c', 'src/player.c', 'src/map.c', 'src/tileset.c', 'lib/hshg/c/hshg.c')
#'src/zuul.c', 'src/zuul_game.c', 'src/zuul_map.c', 'src/zuul_player.c', 'src/zuul_room.c', 'src/zuul_world.c')

executable('zuul', sources, include_directories: incdir, dependencies: deps, c_args : ['-DLOG_USE_COLOR', '-DHSHG_D=2', '-DHSHG_UNIFORM'])

test_sources = files('tests/map_test.c', 'src/map.c', 'lib/log.c/src/log.c', 'src/tileset.c')
map_test = executable('map_test', test_sources, include_directories: incdir, dependencies: deps, c_args: ['-DLOG_USE_COLOR', '-DHSHG_D=2', '-DHSHG_UNIFORM'])
if valgrind.found()
    test('map memory test', valgrind,
         args: ['--leak-check=full', '--error-exitcode=1', map_test.full_path()])
else
    message('Valgrind not found: skipping memory leak tests.')
endif