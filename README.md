# Game Engine

## Introduction

* The application object represents the game loop 
* An application can have more than one scene
* The game loop works on one state only until changeState is called, so game loop changes the scene
* A scene is called a state
* A scene is a group of objects to be drawn
* An object here is a *mesh* + a *material* (together called mesh renderer)
* Mesh has:
  - VAO
  - VBO
  - Draw()
* Material has:
  - program (shaders)
  - pipeline
  - setup()
  - Color (if tint material)
  - Texture and sampler (if texture material)
* The application (or game loop) just calls `onInitialize()`, `onDraw()`  and `setupCallbacks()` of current scene (callback are functions that take input from keyboard and mouse)
* A state has has what it **needs** of and only what it **needs* of anything (object, texture, ..)
* A simple scene can have (one object):
  - Mesh
  - Texture
* A simple scene can have (one object):
  - Mesh
  - Shader + pipeline (instead of material object)
* A more complex one can have (two object):
  - World which has:
    - A camera
    - A Mesh renderer which has:
        - Mesh
        - Texture Material
* A complete scene as play-state has:
  - World has:
    - Entities (entity is a camera or mesh renderer (any object)) have:
  - Renderer has:
    - Render commands: pointers to entities
  - Camera Controller has:
    - Update function that updates camera based on inputs
  - Movement System:
    - Update function that translate entities based on inputs
  - *Last three objects are just refactoring of the game loop*
* To sum up:
  * application calls onDraw of scene
  * Scene calls Draw of mesh
  * repeat


## Responsibilities

| Object | Main Function |
|--|--|
| MeshRenderer | It deserializes mesh and material from json and store them as attributes |
| Camera | It returns projection and view matrices |
| MovementComponent | holds velocities |
| FreeCameraController | holds sensitivity |
| Entity | has components |
| World | has entities |
| Transform | holds translation, rotation and, scale matrices |
| Material | holds shader, pipeline, color, texture and, sampler |
| Pipeline | controls blending, culling and, depth testing |
| Mesh | holds vao, vbo, ebo |
| Vertex | holds color, position, texture Coordinates and, normal |
| MeshUtils | Creates mesh from obj file |
| Shader | Create program, and sends data to shaders |
| MovementSystem | Changes transform matrix using movement velocities |
| FreeCameraController | Updates camera transformation based on mouse motion |
| ForwardRenderer | Renders all objects in correct order and does post processing |
| Sampler | holds sampling properties |
| Texture Utils | loads texture image |
| Texture2d | holds texture object |
| Asset Loader | Loads json config to their corresponding components |
| Application | Controls different states of the game |

## Config File

In config file, scene has:
1. Assets which include:
   1. Meshes
   2. Textures
   3. Material
   4. Shaders
   5. Samplers
2. World which has entities, each entity is composed of some assets (which are called components here) and one of these entities has type camera

Also in config file we describe thw window, start-scene

## Todo

*Light is a component like camera*
*It holds color, type, and angles* see how to describe light completely on learnopengl.com
*take the shader of the last section add it to shaders*
*Create a lit material class that carries light maps* on learnopengl.com
*Add it to forward renderer* `if (config.contains("lighting"))` then create lights 
