# Doldrum: Numbered Feature Library

This is the **human-readable source-of-truth index** for Doldrum features. A matching machine-readable C++ catalog lives in `src/core/FeatureIndex.hpp`.

**Stable ID rule:** keep feature numbers forever; do not reuse deleted numbers or renumber features when adding new ones. Feature IDs are for developer documentation, **not** room tile tokens (`A0`, `A1`, `D0`), card IDs or save file versions.

## Quick index

| ID | Feature | Owning code |
|---|---|---|
| F001 | Player | `entities/Player.cpp` |
| F002 | Floor | `world/Tiles.hpp` |
| F003 | Wall | `world/Tiles.hpp` |
| F004 | Door | `world/Tiles.hpp` |
| F005 | Room | `world/Room.hpp` |
| F006 | Camera | `world/WorldSystem.cpp` |
| F007 | Squib | `entities/Squib.cpp` |
| F008 | Projectile | `combat/Projectile.cpp` |
| F009 | Health | `entities/Squib.cpp` |
| F010 | Sprite Rendering | `render/Renderer.cpp` |
| F011 | Menus | `ui/MenuSystem.cpp` |
| F012 | Save System | `save/SaveSystem.cpp` |
| F013 | World Flow | `world/WorldSystem.cpp` |
| F014 | Application | `main.cpp` |

## Detailed feature entries

### F001 — Player
- **What it does:** WASD movement, normalized diagonal speed, room collisions, and tile-scale body.
- **How it works:** Move player through traversable room tiles; cannot pass walls.
- **Where to edit:** `entities/Player.cpp`
- **Depends on:** Room, Floor, Wall, Game

### F002 — Floor
- **What it does:** A1 walkable ground and eligible spawn surface.
- **How it works:** Floor tile A1 allows movement and room placement.
- **Where to edit:** `world/Tiles.hpp, world/Tiles.cpp, world/Room.cpp`
- **Depends on:** Room, Renderer

### F003 — Wall
- **What it does:** A0 solid tile blocks entities and projectiles.
- **How it works:** Wall tile A0 is solid; wall art may overlay the floor.
- **Where to edit:** `world/Tiles.hpp, world/Tiles.cpp, world/Room.cpp`
- **Depends on:** Room, Player, Squib, Projectile

### F004 — Door
- **What it does:** D0 non-solid entry/exit; currently links rooms by explicit rules.
- **How it works:** Transition between rooms when player enters linked doorway.
- **Where to edit:** `world/Tiles.hpp, world/WorldSystem.cpp`
- **Depends on:** Room, WorldSystem

### F005 — Room
- **What it does:** Load variable-size TXT tile grids; validate dimensions and spawn; report walkable positions.
- **How it works:** Room SIZE W H supports 3..512 each axis; P indicates starting position.
- **Where to edit:** `world/Room.hpp, world/Room.cpp`
- **Depends on:** Floor, Wall, Door

### F006 — Camera
- **What it does:** Follow player within large rooms; clamp to room edges; leave HUD screen-anchored.
- **How it works:** Logical viewport remains 320x192; game world can exceed it.
- **Where to edit:** `world/WorldSystem.cpp, render/Renderer.cpp`
- **Depends on:** Player, Room, Renderer

### F007 — Squib
- **What it does:** First enemy: direct pursuit, wall collision, two HP, contact damage.
- **How it works:** Enemy movement and damage; no pathfinding yet.
- **Where to edit:** `entities/Squib.cpp`
- **Depends on:** Player, Room, Projectile, Game

### F008 — Projectile
- **What it does:** Four-direction shots, rate control, solid collision, enemy hits.
- **How it works:** Starter weapon uses 0.58-second cooldown; sprite DoldShot.png.
- **Where to edit:** `combat/Projectile.cpp`
- **Depends on:** Player, Room, Squib, Renderer

### F009 — Health
- **What it does:** Player HP, hit invulnerability, health HUD and death state.
- **How it works:** Player starts at 6 HP; death enters GameOver screen.
- **Where to edit:** `entities/Squib.cpp, core/Game.hpp, render/Renderer.cpp`
- **Depends on:** Squib, Game, UI

### F010 — Sprite Rendering
- **What it does:** PNG loading, transparent cropping, entity and tile rendering.
- **How it works:** Sprite art from assets/; top-down tile-aligned renderer.
- **Where to edit:** `render/Renderer.cpp, render/DrawUtils.cpp`
- **Depends on:** Room, Camera, Player, Squib, Projectile

### F011 — Menus
- **What it does:** Title, save slots, pause/death screens; mouse and arrows+Enter.
- **How it works:** Menu selections glow; F toggles fullscreen from event loop.
- **Where to edit:** `ui/MenuSystem.cpp, ui/PixelFont.cpp, main.cpp`
- **Depends on:** Save System, Game

### F012 — Save System
- **What it does:** Three persistent slots, elapsed playtime and save-file deletion.
- **How it works:** Version 1 slot files only store playtime; avoid breaking existing saves.
- **Where to edit:** `save/SaveSystem.cpp`
- **Depends on:** Menus, Game

### F013 — World Flow
- **What it does:** Hub-to-dungeon transitions, room changes, encounter placement and update sequence.
- **How it works:** Connections are currently hand-authored, not procedural.
- **Where to edit:** `world/WorldSystem.cpp`
- **Depends on:** Room, Player, Camera, Squib, Projectile

### F014 — Application
- **What it does:** SDL lifetime, events, fixed-step updates and high-level screen state.
- **How it works:** 60Hz simulation; keep main.cpp small and dispatch to feature modules.
- **Where to edit:** `main.cpp, core/AppSystem.cpp, core/Game.hpp`
- **Depends on:** All runtime features

## How to add a new feature

1. Choose the next unused number (currently **F015**). Keep the existing numbers unchanged.
2. Add an entry to `src/core/FeatureIndex.hpp` and update its `std::array` size.
3. Create a focused `.hpp`/`.cpp` module under the appropriate `src/` directory.
4. Add the source to `CMakeLists.txt` if it contains a new `.cpp` file.
5. Add `// FEATURE LIBRARY: F015 Feature Name` to the implementation header.
6. Add a detailed entry to this document describing behavior, dependencies and data files.
7. Add a targeted test if the feature contains game rules or parsing.

## Architecture note

The code is currently **modular C++ source**, not independently built DLLs or static libraries. `core/Game.hpp` still contains shared state. The feature registry is an index for navigation and documentation, **not** a factory or gameplay plugin loader. Future cards can maintain separate stable string IDs and reference feature IDs in developer documentation.
