# Zuul remastered

Simple tiling game originally built in java by me and a classmate.
Now I am trying to recreate it as best I can in C using SDL just for fun.

## Building for linux

### Dependencies

- SDL2
- SDL2 image
- SDL2 ttf
- Meson
- cJSON

Ubuntu
```apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev meson git libcjson```

Arch
```yay -S sdl2 sdl2_image sdl2_ttf meson cjson``` 

```bash
git clone --recurse-submodules https://github.com/erikkallen/zuul-remastered.git
cd zuul
mkdir build
cd build
meson setup ..
ninja
```

## Running

```bash
# Make sure you are in the directory where you built the project
./zuul <path to assets folder>
# If you are in the build folder for example
./zuul ../assets/
```

## Testing
```bash
meson test --wrap=valgrind 'map memory test'
```