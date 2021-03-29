# verbose-chainsaw
Renderer for Wavefront Obj data including material data.

# Features
* Load single obj file and optional mat file
* Renders object with lighting based on provided normals
* Renders object with material data diffuse color if provided
* Interactive Controls

# Planned work
* Add camera rotation around object
* Add lighting movement
* Load textures
* Blender Grid Background

# Reference

## Main_Scene.json
The Main_Scene.json is the config for the scene and the reference is below:

* **obj_filename**: Filename of the obj file. The provided file `Resource\Wavefront\cactus_tall.obj`
  can be used for reference. Note that the filename will need to have double slashes to replace single slashes.
* **mtl_filename**: (Optional) Filename of the material file that is associated with the obj file.
  The provided file `Resource\Wavefront\cactus_tall.mtl` can be used for reference.
* **eye_location**: Start location of the eye. An example for this value can be `[ 0.0, 0.0, 5.0, 0.0 ]`.
* **at_location**: Location to look at. An example for this value can be `[ 0.0, 0.0, 0.0, 0.0 ]`.
* **up_direction**: The up direction for the projection. This should normally be `[ 0.0, 1.0, 0.0, 0.0 ]`.
* **light_direction**: The direction that the light is going. An example for this value can be `[ 0.0, 0.0, 1.0, 1.0 ]`.
* **calculate_normals**: Defines whether to use calculated normals or the provided normals. Set to `true` or `false`.

## Controls
The following are the controls for the scene viewer.

### General
* Number 1 - Toggle between Object and Camera controls.

### Object Mode
* Up Dir - Rotate Object Up around the X Axis
* Down Dir - Rotate Object Down around the X Axis
* Left Dir - Rotate Object positive around the Y Axis
* Right Dir - Rotate Object negative around the Y Axis

### Camera Mode
* Up Dir - Move the camera closer to the object
* Down Dir - Move the camera farther away from the object


# Shader references
https://www.gamasutra.com/view/feature/131275/implementing_lighting_models_with_.php?print=1

https://stackoverflow.com/questions/16986017/how-do-i-make-blender-operate-in-left-hand

https://stackoverflow.com/questions/32053411/normals-rotates-wrong-way-when-object-is-rotated

https://github.com/microsoft/DirectXMesh/blob/master/Utilities/WaveFrontReader.h

https://www.evl.uic.edu/ralph/508S98/coordinates.html
