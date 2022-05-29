
# Game Engine

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
