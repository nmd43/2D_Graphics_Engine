# 2D Graphics Engine in C++

## Contributors: Nikita Daga | Supplementary code: Mike Reed

## Features: 

* This is a 2D Graphics engine that can be used to draw rectangles, triangles, circles, cubic and quadrartic curves, and all convex and concave polygons. 
* These shapes can be filled with different colors, blend modes, opacities, and shading techniques.
* Matrices are used to tranform the images drawn so they can be rotated, inverted, scaled in size, and translated in position. 
* The shapes can be defined in any contour and can be traced correctly and clipped to appropriately fit within the screen.
* Tiling modes for the images are also available (clamp, repeat, and mirror). 
* Different textures can be provided for each vertex to create a triangular or quadrilateral textured mesh.
* All code is optimized to ensure that the graphics engine can render images in a timely manner.

## Notes:

* The /expected folder contains example images that were rendered using the engine
* Tests can be run as:
```bash
$ make tests
$ ./tests -v 
```
* Images can be made as:
```bash
$ make image
$ ./image -e expected -v 
```
* Custom images can be authored under GDrawSomething
