# Doldrum Feature Library (Build 005)
Each feature is designed as a self-contained documented module, not a separate dynamic library. Modules declare their public contract in `src/core/Game.hpp` and implement behavior in dedicated `.cpp` files. This first extraction prioritizes compatibility; future builds will split the shared header into narrow interfaces.

| Feature | Responsible file | Behavior and dependencies |
|---|---|---|
| Player | `world/WorldSystem.cpp` | Move by normalized WASD vector at PLAYER_SPEED; collision samples corners against Room::isSolid. Four-direction shooting and health currently share this module. Depends on Room and Game. |
| Floor | `world/Room.cpp` | Tile `A1`; walkable, eligible for enemy/item placement; art in `assets/backgrounds/Floor.png`. |
| Wall | `world/Room.cpp` | Tile `A0`; impassable to players, squibs and shots. Visuals in `assets/tiles/Wall.png`. |
| Door | `world/Room.cpp`, `WorldSystem.cpp` | Tile `D0`; traversable only where a room connection has been configured. For now hub to dungeon and dungeon 1 to dungeon 2 are manually mapped. |
| Room | `world/Room.hpp/.cpp` | TXT parser accepts arbitrary SIZE 3..512 per dimension; row/column indices and spawnable tiles are per-room. Original TXT files still load. |
| Camera | `world/WorldSystem.cpp` and `render/Renderer.cpp` | Follows player, clamps at map bounds, transforms world coordinates to 320x192 screen; HUD remains screen fixed. |
| Squib | `world/WorldSystem.cpp` | Direct pursuit at fixed speed, 2 HP, collision damage; wall collision; no pathfinding. |
| Projectiles | `world/WorldSystem.cpp` | Four directions, 0.58-second rate, wall and enemy impact, existing art. |
| Sprites | `render/Renderer.cpp` | Loads PNGs through SDL3_image, clips transparent margins, draws nearest-neighbor sprites at 16x16 top-down size. |
| UI/menus | `ui/MenuSystem.cpp`, `ui/PixelFont.cpp` | Title/save/pause/death buttons; keyboard and mouse interaction. |
| Saves | `save/SaveSystem.cpp` | Three on-disk version 1 slots storing elapsed playtime; no format changes this build. |
| Application | `main.cpp`, `core/AppSystem.cpp` | SDL lifecycle, event routing, fixed 60Hz updates and frame dispatch. |

## Room authoring
Change `SIZE W H` and supply exactly H lines of W space-delimited token codes after TILES. The P spawn is mandatory. D0 border doors must match DOORS declarations. 16 logical pixels per tile. Camera shows 20x12 tiles at a time. The new sample `room_large.txt` is 44x28 and replaces the east test room for now.

## Important limitations
- Existing Room loader only understands A0, A1, D0, P.
- Inter-room links are still explicitly wired, not procedural.
- Shared Game structure is still a central dependency; further interface isolation comes after gameplay verification.
- Floor/wall art is clipped to opaque bounds. If tiles use intentionally transparent padding, consider disabling cropping specifically for seamless tiles in a later art pass.
- Game saves store playtime, not run/world state.

## Narrow feature implementations
- `entities/Player.cpp`: player collision and movement; 16px sprite and ~10px collision footprint.
- `entities/Squib.cpp`: enemy pursuit, enemy damage, and collision to shots.
- `combat/Projectile.cpp`: firing cooldown and bullet-wall collisions.
- `world/Tiles.hpp/.cpp`: Floor, Wall and Door types are individual code-level definitions with stable IDs and solidity rules; the room parser consults the tile policy.
- `world/WorldSystem.cpp`: only room changes, enemy placement, top-level sequencing and camera.
