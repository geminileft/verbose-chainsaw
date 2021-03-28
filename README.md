# verbose-chainsaw
Renderer for Wavefront Obj data including material data.

# Features
* Load single obj file and optional mat file
* Renders object with lighting based on provided normals
* Renders object with material data diffuse color if provided
* Movement of object using up / down / left / right keys

# Planned work
0. Document
1. Add camera movement ( fixed / free )
1. Add lighting movement
2. Generate mesh normals
2. Load textures
3. Blender Grid Background

# Main_Scene.json Reference
The Main_Scene.json is the config for the scene and the reference is below:

* **obj_filename**: Filename of the obj file. The provided file `Resource\Wavefront\cactus_tall.obj`
  can be used for reference. Note that the filename will need to have double slashes to replace single slashes.
* **mtl_filename**: Filename of the material file that is associated with the obj file.
  The provided file `Resource\Wavefront\cactus_tall.mtl` can be used for reference.
* **eye_location**: Start location of the eye. An example for this value can be `[ 0.0, 0.0, 5.0, 0.0 ]`.
* **at_location**: Location to look at. An example for this value can be `[ 0.0, 0.0, 0.0, 0.0 ]`.
* **up_direction**: The up direction for the projection. This should normally be `[ 0.0, 1.0, 0.0, 0.0 ]`.
* **light_direction**: The direction that the light is going. An example for this value can be `[ 0.0, 0.0, 1.0, 1.0 ]`.

## Shader references
https://www.gamasutra.com/view/feature/131275/implementing_lighting_models_with_.php?print=1

https://stackoverflow.com/questions/16986017/how-do-i-make-blender-operate-in-left-hand

https://stackoverflow.com/questions/32053411/normals-rotates-wrong-way-when-object-is-rotated

https://github.com/microsoft/DirectXMesh/blob/master/Utilities/WaveFrontReader.h

https://www.evl.uic.edu/ralph/508S98/coordinates.html
