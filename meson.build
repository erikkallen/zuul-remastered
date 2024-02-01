project('zuul', 'c')

incdir = include_directories('include', 'lib/log.c/src')
sdl2_dep = dependency('sdl2')

executable('zuul', 'main.c', 'lib/log.c/src/log.c', include_directories: incdir, dependencies: sdl2_dep, cpp_args : '-DLOG_USE_COLOR')
