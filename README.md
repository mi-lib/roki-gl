RoKi-GL - Robot Kinetics library: visualization using OpenGL
=================================================================
Copyright (C) Tomomichi Sugihara (Zhidao) since 2000

-----------------------------------------------------------------
## [What is this?]

RoKi-GL is a library to visualize robots and 3D shapes using
penGL. It includes
- camera work
- optics
- visualization of shapes
- visualization of kinematic chains
- visualization of terrain models
- object selection
- handle to manipulate objects
- texture mapping
- bump mapping
- reflection and refraction mapping
- shadow mapping
- GLUT (OpenGL utility toolkit) wrapper
- X11/OpenGL wrapper
- shaders using GLSL

ZEDA, ZM, Zeo, RoKi, LIW and ZX11 are required.

-----------------------------------------------------------------
## [Installation / Uninstallation]

### install

Install the following libraries in advance.
- GL
- GLU
- GLUT (option)
- GLX (option)
- GLEW (option)
- ZEDA
- ZM
- Zeo
- RoKi
- LIW
- ZX11

Move to a directly under which you want to install RoKi-GL, and run:

   ```
   % git clone https://github.com/zhidao/roki-gl.git
   % cd roki-gl
   ```

Edit **PREFIX** in *config* file if necessary in order to specify
a directory where the header files, the library and some utilities
are installed. (default: ~/usr)

   - header files: $PREFIX/include/roki_gl
   - library file: $PREFIX/lib
   - utilities: $PREFIX/bin

Also, edit **CONFIG\_USE\_GLX** and **CONFIG\_USE\_GLUT** if you
don't use either.

Then, make and install.

   ```
   % make && make install
   ```

### uninstall

Do:

   ```
   % make uninstall
   ```

which removes $PREFIX/lib/libroki-gl.so and $PREFIX/include/roki_gl.

-----------------------------------------------------------------
## [How to use]

When you want to compile your code *test.c*, for example, the following
line works.

   ```
   % gcc `roki-gl-config --cflags` test.c `roki-gl-config -l`
   ```

-----------------------------------------------------------------
## [Contact]

zhidao@ieee.org
