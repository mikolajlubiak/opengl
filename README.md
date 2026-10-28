# opengl

An OpenGL 4.6 core-profile renderer: it loads a textured 3D model with Assimp and lights it with directional, point and spot lights (the point light moves and changes color), viewed through a free-look camera.

## Build

Tested on Linux.

- Install necessary packages (different commands based on your distribution)
  - Fedora:
    - `sudo dnf install gcc-c++ glfw glfw-devel glm-devel assimp assimp-devel stb_image-devel`
  - Arch:
    - `sudo pacman -S --needed gcc glfw glm assimp stb`
  - Ubuntu:
    - `sudo apt install g++ make libglfw3-dev libglm-dev libassimp-dev libstb-dev`
- `git clone https://github.com/mikolajlubiak/opengl`
- `cd opengl`
- `make debug`
- `./opengl`

## Other branches

| Branch | Description |
|--------|-------------|
| [`conways-game-of-life`](https://github.com/mikolajlubiak/opengl/tree/conways-game-of-life) | Conway's Game of Life rendered in 3D — the simulation runs on a 2D grid while each successive generation is stacked along the Y axis, making game history the third spatial dimension. |

## Controls

| Key / Input        | Action                  |
|--------------------|-------------------------|
| `W` `A` `S` `D`   | Move camera             |
| Mouse              | Look around             |
| Scroll wheel       | Zoom in / out           |
| `Escape`           | Quit                    |
