# To Run The Game

```bash
./bin/GAME_APPLICATION -c="config/game.jsonc"
```

# Game Engine Overview

* The `Application` object represents the game loop 
* An `Application` can have more than one scene
* The game loop works on one `State` only until `changeState()` is called, so game loop changes the scene
* A scene is called a `State`
* A scene is a group of objects to be drawn
* An object here is a `Mesh` + a `Material` (together called `MeshRenderer`)
* `Mesh` has:
  - `VAO`
  - `VBO`
  - `Draw()`
* `Material` has:
  - `Program` (shaders)
  - `Pipeline` (depth testing settings + face culling settings)
  - `setup()`
  - `Color` (if `TintMaterial`)
  - `Texture` and `Sampler` (if `TextureMaterial`)
* The `Application` (or game loop) just calls `onInitialize()`, `onDraw()`  and `setupCallbacks()` of current scene (callback are functions that take input from keyboard and mouse)
* A `State` has has what it **needs** of and only what it **needs* of anything (object, texture, ..)
* A simple `State` can have (one object):
  - `Mesh`
  - `Texture`
* A simple `State` can have (one object):
  - `Mesh`
  - `Shader` + `Pipeline` (instead of material object)
* A more complex one can have (two object):
  - `World` which has:
    - A `Camera`
    - A `MeshRenderer` which has:
        - `Mesh`
        - `TextureMaterial`
* A complete `State` as "play-state" has:
  - `World` has:
    - Entities (`Entity` is like a `Camera` or `MeshRenderer` (any object)) have:
  - Renderer (as `ForwardRenderer`) has:
    - Render commands: pointers to entities
  - `CameraController` has:
    - Update function that updates camera based on inputs
  - `MovementSystem`:
    - Update function that translate entities based on inputs
  - *Last three objects are just refactoring of the game loop*
* To sum up:
  * `Application` calls `onDraw()` of scene
  * `State` calls `draw()` of `Mesh`
  * repeat


## Class Responsibilities

| Object | Main Function |
|--|--|
| `MeshRenderer` | It deserializes mesh and material from json and store them as attributes |
| `Camera` | It returns projection and view matrices |
| `MovementComponent` | It holds velocities |
| `FreeCameraController` | It holds mouse sensitivity and updates camera transformation based on mouse motion |
| `Entity` | It has components |
| `World` | IT has entities |
| `Transform` | It holds translation, rotation and, scale matrices |
| `Material` | It holds shader, pipeline, color, texture and, sampler |
| `Pipeline` | It controls blending, culling and, depth testing |
| `Mesh` | It holds vao, vbo, ebo |
| `Vertex` | It holds color, position, texture Coordinates and, normal |
| `MeshUtils` |It Creates mesh from obj file |
| `Shader` | It create program, and sends data to shaders |
| `MovementSystem` | It changes transform matrix using movement velocities |
| `ForwardRenderer` | It renders all objects in correct order and does post processing |
| `Sampler` | It holds sampling properties |
| `TextureUtils` | It loads texture image |
| `Texture2d` | It holds texture object |
| `AssetLoader` | It Loads json config to assets vectors and creates components from their assets (more like a factory)|
| `Application` | Controls different states of the game |

## Config File

In config file, scene has:
1. Assets which include:
   1. Meshes
   2. Textures
   3. Material
   4. Shaders
   5. Samplers
2. World which has entities, each entity is composed of some assets (which are called components here) and one of these entities has type camera (which can have children that moves with it)

Also in config file we describe the window dimensions, start-scene (but scenes (states) themselves are hared-coded)
