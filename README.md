# Doldrum Build 005 - Zelda top-down / Modular architecture
Open with Visual Studio CMake (windows-debug). The room system now reads variable-size maps, the camera follows the player within a room, and the player sprite is one 16x16 tile in top-down view. Source files are organized by feature responsibility. See `docs/FEATURE_LIBRARY.md` for the exact behaviors and dependencies.

**Install:** Copy project files over Build 004, preserve `.git` and existing PNG assets. `room_large.txt` replaces the previous eastern room for a camera demonstration. SDL3_image AVIF is disabled to avoid NASM configuration issues.

**Controls:** WASD walk, arrow keys fire, Escape pause, F fullscreen; mouse or arrows+Enter navigate menus.

**Limitations:** The new code has not been compiled on Windows in this environment. Save files remain compatible (version 1, playtime). Room door topology remains scripted; the next pass will generalize room links. Commit after testing.

## Numbered feature index

See `docs/FEATURE_LIBRARY.md` (F001–F014) and `src/core/FeatureIndex.hpp` for the searchable, numbered code index. Each source module has a `FEATURE LIBRARY` comment linking it to its assigned feature ID. This adds documentation only; gameplay is unchanged.
