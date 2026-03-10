# opengl

Me learning OpenGL — renders a 3D model with dynamic lighting (directional, point, and spot lights) and a free-look camera.

## Build

Tested on Linux.

- Install necessary packages (different commands based on your distribution)
  - Fedora:
    - `sudo dnf install gcc-c++ glfw glfw-devel glm-devel assimp assimp-devel`
  - Arch:
    - `sudo pacman -S --needed gcc glfw glm assimp`
  - Ubuntu:
    - `sudo apt install g++ libglfw3-dev libglm-dev libassimp-dev`
- `git clone https://github.com/mikolajlubiak/opengl`
- `cd opengl`
- `make debug`
- `./opengl`

## Controls

| Key / Input        | Action                  |
|--------------------|-------------------------|
| `W` `A` `S` `D`   | Move camera             |
| Mouse              | Look around             |
| Scroll wheel       | Zoom in / out           |
| `Escape`           | Quit                    |
