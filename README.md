# verbose-chainsaw
Renderer for Wavefront Obj data including material data.

# Features
* Load single obj file and optional mat file
* Renders object with lighting based on provided or calculated normals
* Renders object with material data diffuse color if provided
* Interactive Controls

# Planned work
* Add UI for fixed / follow lighting
* Load textures
* Blender Grid Background
* Lighting Refactor


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
* **light_direction**: (Optional) The direction that the light is going. An example for this value can be `[ 0.0, 0.0, 1.0, 1.0 ]`.
* **calculate_normals**: (Optional) Defines whether to use calculated normals or the provided normals. Set to `true` or `false`.

## Controls
The following are the controls for the scene viewer.

### General
* Number 1 - Toggle between Object and Camera controls.
* L - Toggle between fixed and follow lighting.

### Object Mode
* Up Dir - Rotate Object Up around the X Axis
* Down Dir - Rotate Object Down around the X Axis
* Left Dir - Rotate Object positive around the Y Axis
* Right Dir - Rotate Object negative around the Y Axis

### Camera Mode
* Up Dir - Move the camera closer to the object
* Down Dir - Move the camera farther away from the object
* A - Move the camera around the object counter clockwise around the Y Axis
* D - Move the camera around the object clockwise around the Y Axis
* W - Move the camera around the object clockwise around the X Axis
* S - Move the camera around the object counter clockwise around the X Axis


# Web references
https://www.gamasutra.com/view/feature/131275/implementing_lighting_models_with_.php?print=1

https://github.com/microsoft/DirectXMesh/blob/master/Utilities/WaveFrontReader.h
