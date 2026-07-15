# 3D Terrain Rendering Project

A 3D graphics project demonstrating terrain generation, physics simulation, advanced rendering techniques, and procedural object placement. The project combines multiple real-time rendering features including lighting, shading, animation, and GPU-based effects.

## Features

### Procedural Terrain Generation
- Terrain is generated from a grayscale heightmap image.
- Uses tessellation to dynamically add surface detail.
- Terrain elevation is used for physics interactions and object placement.

### Physics and Environment Simulation
- Gravity simulation with:
  - Walking on terrain
  - Swimming
  - Jumping
  - Running
- Water surface simulation with animated waves.
- Skybox environment rendering.

### Rendering Techniques

#### Blending
- Alpha blending implemented for:
  - Water waves
  - Transparent windows
- Correct rendering order is handled to ensure proper transparency.

#### Stencil Effects
- Object outlining implemented using the stencil buffer.

#### Geometry Shader Effects
- Explosion effect implemented using geometry shaders.

#### Advanced Lighting
Complete lighting system supporting:
- Ambient lighting
- Diffuse lighting
- Specular lighting
- Directional lights
- Point lights
- Spot lights

### Materials and Shading
- Normal mapping and specular mapping implemented for stone surfaces.
- Models can use multiple textures across different meshes.

### Procedural Object Placement
- Random generation and placement of:
  - Grass
  - Trees
  - Windows above the water surface

### Model Animation and Interaction
- Mesh-based model animation:
  - Helicopter model with independently moving mesh components.
- Skull models that continuously track and face the camera direction.
- Animated models without relying on textures.

## Technologies Used

- C++
- OpenGL
- GLSL Shaders
- Tessellation Shaders
- Geometry Shaders
- Stencil Buffering
- Normal Mapping
- Specular Mapping
- Procedural Generation
- Real-time Lighting

## Running the Project

1. Configure the project using CMake.
2. Build the project.
3. Run the generated executable.
