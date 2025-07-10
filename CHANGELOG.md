# Changelog

All notable changes to the engine will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0-dev] - 2025-07-10

### Added
- Enabled depth testing in the window class by default
- Added an "enabled" boolean in the root Component class
- Added a brand new SpriteAnimator Component and SpriteAnimationSystem
  - SpriteAnimator stores animations composed of multiple sprites, and settings like frame time and loop.
  - SpriteAnimationSystem grabs the current animated sprite and sets it as the active texture in the linked Sprite.material of the entity to create the animation on screen over time.
- assets/default.pak folder for base assets, and assets/shaders.pak for more complex shaders that could be used.

### Changed
- ComponentStorage can now manage hold multiple components of the same type for a given entity. (this allows behaviourSystem to run multiple scripts per entity, could be used in other systems in the future)
  - Entity and Registry classes now have a new "GetComponents" method to get multiple components of a given type. 
  - "GetComponent" returns the first one in the list to maintain compatibility.
- Material can now return current uniforms
- Entity now has a "ToString" method for debugging prints
- The root App class no longer loads defaults/paks/systems. This behaviour is meant to go in the derived App class in the main entry point of the app.

## [1.1.0-dev] - 2025-07-02

### Added
- Added Frustum Culling for sprites rendering based on a 2D Camera

### Removed
- Removed docs from tha main branch of the engine.

## [1.0.0-dev] - 2025-07-02

### Added
- Base src/engine code to get started with opengl wrapper
- A basic src/main.cpp with src/scenes/hello_world.hpp|.cpp to act as an example
- Updated docs/ folder and built basic README file.
