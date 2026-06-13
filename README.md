# Bomberman - Custom C++ Engine Project

## Overview

For this project I developed a Bomberman clone using my own custom C++ game engine built on top of SDL3. The goal of the project was not only to recreate Bomberman gameplay, but also to design a reusable engine architecture that could support multiple games and game states.

The engine provides functionality for:

- Scene management
- Component-based GameObjects
- Input handling
- Audio playback
- State management
- Physics and collision detection
- Resource management
- Observer/Event systems
- Save file handling

Throughout the project I tried to prioritize readability, maintainability and separation of responsibilities over premature optimization. That ws in the beginning, latter stages were truly rushed, reminding me of a gameJam... The code needs quite a bit of polishing in any case.

## Engine Architecture

### Component-Based Design

GameObjects themselves contain very little logic. Instead, functionality is split into reusable components.

Examples include:

- TransformComponent
- RenderComponent
- PhysicsComponent
- TextComponent
- FPSComponent
- etc.

This approach allows objects to be composed from behaviors rather than relying on large inheritance hierarchies.

### Scene System

The engine uses a Scene / SceneManager architecture.

The SceneManager owns all active scenes and is responsible for updating and rendering them.

Each Scene owns its GameObjects using `std::unique_ptr`, guaranteeing automatic cleanup and preventing memory leaks.

### State Stack

To manage menus and gameplay I implemented a GameState stack. My favorite part I believe...

Current states include:

- Title Screen
- Main Menu
- In Game
- Game Over
- Leaderboard

The stack supports Push, Pop and Change operations and allows transitions to be queued safely.

## Input System

The input system is command based.

Instead of binding gameplay logic directly to controller buttons, buttons execute Command objects.

Examples include:

- MoveCommand
- ChangeStateCommand
- ExecuteCallbackCommand (late and powerful addition, let's me do anything)

This design decouples input devices from gameplay systems.

## Physics System

The physics system is intentionally lightweight.

Objects use axis-aligned bounding boxes and collision checks are performed through rectangle intersection tests.

Since Bomberman does not require realistic physics simulation, a custom solution was sufficient and easier to debug.

## Observer Pattern

Several gameplay systems communicate through the Observer pattern.

Examples include:

- Enemy death notifications
- Player death notifications
- Score updates

This reduces coupling between gameplay systems and improves maintainability.

## Level Generation

Levels are loaded from CSV files.

The level is converted into a LevelGrid structure containing GridCells.

Each GridCell stores:

- Grid coordinates
- World position
- Cell type
- Occupants

The occupant system became a central part of gameplay because it allows bombs, blasts, barrels and upgrades to interact through the grid.
Kind of a procedural way? Nice.

## Bomb and Blast System

Bombs are updated separately from scene objects and managed through the InGameState.

When a bomb explodes:

1. The bomb marks itself for deletion.
2. Blast cells are spawned.
3. Blast occupants are registered inside affected GridCells.
4. Barrels are destroyed.
5. Chain reactions are triggered.

Because Bomberman is grid based, explosion propagation is also grid based rather than radius based.

## Upgrade System

Upgrades are hidden inside destructible barrels.

Available upgrades include:

- Increased bomb count
- Increased blast range
- Increased movement speed

## Enemy AI

For this project I implemented Balloom enemies.

Ballooms use simple random movement:

1. Choose a valid neighboring cell.
2. Move in that direction.
3. If blocked, choose another direction.
4. Kill players on contact.
5. Die when touching a blast.

This behavior accurately reflects the original Bomberman design.

## Save System

The leaderboard uses a custom binary save format.

Each save file stores:

- Entry count
- Player names
- Scores

Binary serialization was chosen because it is compact and fast to load.

## Memory Management

The engine relies heavily on modern C++ ownership patterns.

Key containers and utilities include:

- std::unique_ptr
- std::vector
- std::array

Raw pointers are only used as non-owning references.

## Design Decisions

### Grid-Based Gameplay

Many systems operate directly on GridCells, simplifying:

- Explosions
- Upgrade spawning
- Enemy movement
- Chain reactions

### Composition Over Inheritance

Whenever possible I preferred composition through components instead of deep inheritance hierarchies.

### Simplicity Over Overengineering

Many systems could have been made more generic or complex. Instead, I focused on solutions that matched the requirements of the project while remaining easy to debug and maintain.

## Conclusion

This project allowed me to apply software engineering concepts including component-based architecture, state machines, command patterns, observer systems, resource management and custom serialization.

The final result is a complete playable Bomberman game supported by a custom C++ engine that I developed and extended throughout the project.

## Controls

No keyboard bindings this time... At least not yet (hopefully to appear during summer)
Must Be played on a controller:

DPAD - Movement. 
A - Select/Press/Drop bomb
B - Go back in some menus
Select - Skip levels

I may have forgotten something, but i am heavily sleep-deprived... 

Thank you for reading :)
