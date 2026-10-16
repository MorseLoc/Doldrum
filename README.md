# Doldrum | Build 002

C++20 + SDL3 dungeon-crawler prototype for Visual Studio CMake.

## Run

1. Open this folder in Visual Studio via **File > Open > Folder**.
2. Select the `windows-debug` CMake configure preset, and then the Doldrum startup target.
3. Press **F5**. On first configure, CMake may need to download SDL3.
4. Edit `data/rooms/room_start.txt` and rebuild to change the starting room.

If replacing Build 001 in your local repository, copy the contents of this folder over the old ones (keep your `.git` directory), then commit the changes. Build results in `out/` are ignored.

## Controls

- WASD: movement (normalized diagonals)
- Arrow keys: shoot in four directions
- Walk through the **gold door on the east side** of the starting room to reach the second room; go back through its west door to return.
- Escape: exit

## Room TXT format

Every room is a 20-wide by 12-high grid, with one 16x16 logical pixel tile per cell. Example:

```text
ROOM example_id
SIZE 20 12
DOORS EAST
TILES
A0 A0 ... (20 entries total)
... (12 rows total)
```

Tiles:

- `A0`: solid wall
- `A1`: walkable floor
- `D0`: passable door tile, usually on a room boundary
- `P`: player spawn, exactly once per room; internally converted to floor

`DOORS` declares one or more of `NORTH EAST SOUTH WEST`. The loader ensures a declared doorway has a corresponding `D0` on the relevant edge. The initial two rooms are explicitly connected in code; procedural room connectivity comes in Build 004.

The `Room` model exposes `walkable` coordinates and `freeTiles(occupied)` for future bank, enemy, and item placement. For future content, extend tile behavior in one location rather than putting gameplay entities into map token IDs.

## Technical notes

- Room parsing is in `src/world/Room.cpp`, separate from SDL and rendering.
- CMake copies `data/` beside the game executable after each build. Rooms load relative to `SDL_GetBasePath()`.
- Both the player and bullets collide with solid walls. The collision shape remains a simple placeholder.
- The room loader rejects invalid sizes, missing spawn points, malformed rows, duplicate spawns, unknown tile codes, and missing declared door tiles.
- Build 002 focuses on room systems, not procedural generation, enemy encounters, or final graphics.

## Optional parser sanity test

On a system with g++:

```bash
g++ -std=c++20 -Isrc tests/room_test.cpp src/world/Room.cpp -o room_test
./room_test data/rooms/room_start.txt data/rooms/room_east.txt
```
