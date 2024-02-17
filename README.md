# Zuul remastered

Simple tiling game originally built in java by me and a classmate.
Now I am trying to recreate it as best I can in C using SDL just for fun.

![Screenshot of zuul](/assets/screenshot.png)

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
# Copy the assets.json to the directory you built the projects e.g. <project_root>/build
cp assets/assets.json build/
# Update paths in the assets file if needed
sed -i 's|"path": "../assets"|"path": "./new_assets_path"|g' build/assets.json
# Make sure you are in the directory where you built the project
./zuul
```

## Testing

```bash
meson test --wrap=valgrind 'map memory test'
```

## Map making

For mapmaking I used Tiled. Currently the following features are supported in the engine:

- Multiple layers
- Animations using the tiled animation editor
- Multiple size tiles should work (tested 32, 16 and 128px)
- Primitive map loading using objects with a string property called "warp" and the value is the name of the map and coordinates on the destination map: map.tmj:x,y
- 

## Thanks to the following projects for their awesome tools/libraries/inspiration

- [Tiled](https://www.mapeditor.org/)
- [Pixelorama](https://github.com/Orama-Interactive/Pixelorama)
- [SDL](https://www.libsdl.org/)
- [Kenny](https://www.kenney.nl/assets/roguelike-rpg-pack)
- [cJSON](https://github.com/DaveGamble/cJSON)
- [log.c](https://github.com/rxi/log.c)
- 