RoKi-GL - Robot Kinetics library: visualization using OpenGL
Copyright (C) 2000 Tomomichi Sugihara (Zhidao)

-----------------------------------------------------------------
[What is this?]

RoKi-GL is a library to visualize robots and 3D shapes using
OpenGL. It includes
 - camera work
 - optics
 - visualization of shape
 - visualization of kinematic chain
 - object selection
 - shadow map
 - GLUT (OpenGL utility toolkit) wrapper
 - X11/OpenGL wrapper

ZEDA, ZM, Zeo, RoKi, LIW and ZX11 are required.

-----------------------------------------------------------------
[Installation / Uninstallation]

<install>
0. Install ZEDA, ZM, Zeo, RoKi, LIW and ZX11 in advance.

1. Unpack the distributed archive where you want.

% zcat roki-gl-X.Y.Z.tgz | tar xvf
or, if you use GNU tar,
% tar xzvf roki-gl-X.Y.Z.tgz

X.Y.Z is for the revision.

2. Enter the directory.

% cd roki-gl-X.Y.Z

3. Edit config file if necessary.
  PREFIX   directory where the library is installed.
           ~/usr as a default. In this case, header files
           and library are installed under ~/usr/include
           and ~/usr/lib, respectively.

4. Make it.

% make

5. Install it.

% make install

Or,

% cp -a lib/libroki-gl.so $PREFIX/lib/
% cp -a include/roki-gl $PREFIX/include/
% cp -a bin/* $PREFIX/bin/

<uninstall>
Delete $PREFIX/lib/libroki-gl.so and $PREFIX/include/roki-gl.

-----------------------------------------------------------------
[How to use]

You may need to set your PATH and LD_LIBRARY_PATH environment
variables. This is done by:
 export PATH=$PATH:$PREFIX/bin
 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PREFIX/lib
if your working shell is Bourne shell (bash, zsh, etc.), or by:
 set path = ( $path $PREFIX/bin )
 setenv LD_LIBRARY_PATH $LD_LIBRARY_PATH:$PREFIX/lib
if your working shell is C shell (csh, tcsh, etc.).

When you want to compile your code test.c, for example, the following
line will work.

% gcc `roki-gl-config -L` `roki-gl-config -I` test.c `roki-gl-config -l`

-----------------------------------------------------------------
[Contact]

zhidao@ieee.org
