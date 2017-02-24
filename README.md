# CG-Raytracer
Extras: 
 - Anti-Aliasing
 - Depth of field
 - Motion Blur 
 - Optimisations: OpenMP, Möller–Trumbore with Cramer's rule and a bit of triangle precomputation (just edged at the moment), O3 and some simplifications. 
 - Soft shadows
 - Fully reflective (mirror) surfaces with multiple reflection bounces (Depth can be set in the raytracer)
 - Texture mapping. Loads of texture files can be loaded (jpg, png, bmp, etc). We are converting the CImg to a custom DS that is cache friendly.

Textures require (they can be disabled by changing the define statement from the code and it will compile just fine without them): 
  - CImg library, which requires:
  	- jpeg c library
  	- X11 library (probably already there, but needs to have the right paths)



