# CG-Raytracer
Extras: 
 - Anti-Aliasing 
 - Depth of field (the camera's aperture is modeled as being a square and random points are sampled)
 - Motion Blur (if an object changes position from a frame to another, dt is computed and each ray is sent a random time (from 0 to dt))
 - Optimisations: OpenMP, O3 and some simplifications, Möller–Trumbore with Cramer's rule and precomputation of edges
 - Soft shadows (with random sampling)
 - Fully reflective (mirror) surfaces
 - Texture mapping. Uses CImg to load images (can be pretty much anything, jpgs, pngs, bmps, etc). The images are then converted to data structures that are chache friendly.
 - Normal maps
 - .obj loader which is capable of loading any trianglulated objects with their UV maps and vertex normals
 - KD Trees (WIP) 
 - Normal interpolation (WIP)


Textures require (they can be disabled by changing the define statement from the code and it will compile just fine without them): 
  - CImg library, which requires:
  	- jpeg c library
  	- X11 library (probably already there, but needs to have the right paths)



