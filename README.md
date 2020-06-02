# Basic Raycaster

This program renders a basic 3d scene using raycasting. It supports spheres that can be opaque, reflective, or transparent, and a single light source, and the camera can only point in a single direction.

## Usage

Basic usage is `raycaster <description file> <width> <height> <outputfile>`. Leave the extension off of the last parameter; if the description file describes a sequence of frames, then the frame number will be append as a suffix to the outputfile before the extension (right now only `.tga`). Once the frames have been rendered, they can be combined into a video with, for example,

`ffmpeg -framerate 60 -i img%03d.tga video.mp4`

where the framerate is customizable and it uses frames of the form `img000.tga` through `img999.tga`, in order, to produce the mp4.

## Description File Format

The file should start with a single line containing three integers. The first represents how many frames are described below. The second two integers are both either 0 or 1, which are used as boolean flags for fixed camera and fixed light source, respectively. If fixed camera is 0, for example, then the location of the camera must be described in every frame (and the same is true for the light source).


If the camera flag is 1, then the position of the camera must be specified as a coordinate septuplet on its own line:

```
1 1 0
x y z dx dy dz dt
```

where `(x, y, z)` is the center of the camrea, `(dx, dy, dz)` is the direction vector of the camera, and `dt` is the rotation of the camera around its direction vector (in degrees). If `dt=0`, for example, then a row of pixels is perpendicular to the `z` axis, and if `dt=pi`, then it is flipped upside down.

If the light flag is 1, then the position of the camera must be specified as a coordinate triplet and color triplet (integers 0-255). For example,

```
1 0 1
10 10 10 255 255 255
```

is a camera at `(10, 10, 10)` that lets out full-bright white lights.

If both camera and light are fixed, then it must be the camera first, then light.

After the header, the following must be specified for each frame, each on its own line on the following order

* Number of objects in scene -- `N`
* (Optional) Orientation of camera (as described above) -- only if fixed camera flag is 0
* (Optional) Orientatoin of light (as described above) -- only if fixed light flag is 0
* The description of `N` objects.

A sphere, at its minimum, must include its position and radius as follows:

```
SPHERE
LOCATION a b c R
```

where the triplet `(a, b, c)` is the position of the center of the sphere and `R` is the radius. There are also optional parameters that will be set to 0 if not included. These optional parameters, in the order they should be included, are:

* Color/Texture -- Either an `(r, g, b)` triplet (each in [0, 255]) or a path to a valid texture image.
* Normal Map -- Path to a valid texture image that will be parsed as a normal map
* Reflectivity -- Double in [0, 1] indicating how reflective the sphere is.
* Refractivity -- Double in [0, 1] indicating how refractive the sphere is.
* Index -- Positive double indicating how much light bends when entering/exiting the sphere. 
* Rotation -- Doubles indicating rotation of the sphere along the `x`, `y`, and `z` axis, respectively, in degrees.

These optional parameters must be specified on their own line as follows:

```
COLOR r g b
TEXTURE file/path.tga
REFLECTIVITY 1
REFRACTIVITY 1
INDEX 1.4
ROTATION 180 180 90
NORMAL file/path2.tga
```

The lines describing a sphere must end with a line containing `DONE` by itself.

## Multithreading

Without any multithreading, running the program at 640x480 resolution on three spheres, one opaque, one reflective and refractive, and one reflective with a texture, gave the following output:

```
real    6m15.841s
user    6m11.104s
sys     0m3.901s
```

With a thread processing each row of pixels, that turned into

```
real    1m37.366s
user    5m37.548s
sys     0m15.984s
```

which is almost 4 times faster. Further optimizations reduced it to about 35 seconds real time.