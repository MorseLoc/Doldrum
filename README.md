# Doldrum Build 003: Menus and save slots

This is an **overlay update** for the existing Build 002 GitHub repository. Copy these files over the existing project, preserving the existing `src/world/Room.cpp`, `src/world/Room.hpp`, `data/rooms/room_start.txt`, `data/rooms/room_east.txt`, and `CMakePresets.json`.

## Features
- Animated block-letter DOLDRUM title, light-up / fade UI buttons and mouse support
- Main Menu: Play Game, Quit, F fullscreen toggle
- Three persistent save slots and playtime counters
- Empty slot creates a new save, existing slot loads the hub
- Two-step deletion: DELETE FILE, then choose a slot, then CONFIRM DELETE; BACK cancels
- Hub TXT room with north doorway into the existing two-room dungeon
- Esc in hub or dungeon opens pause overlay; Resume and Quit to Menu
- Play time autosaves approximately every 10 seconds and when changing screens / quitting
- Organized folders under assets/ to begin adding pixel art

## Controls
WASD move; arrow keys shoot; mouse hover / click menu buttons; Esc pause/back; F fullscreen.

## Save location
The game uses SDL_GetPrefPath("MorseLoc", "Doldrum") to save slot_1.txt, slot_2.txt, slot_3.txt to a writable user application data directory. These files are not included in GitHub. Save format is a small versioned prototype text format, designed to be expanded later.

## Working with your GitHub repo
Open the existing Doldrum project folder in Visual Studio. Copy the overlay files in, allow CMake to reconfigure, build and run. **Do not delete the existing src/world or original data/rooms files.** Commit/push after confirming it compiles and behaves as expected.

## Pixel-art folders
assets/characters, enemies, tiles, props, projectiles, items, cards, ui, fonts, effects, backgrounds.
The temporary bitmap UI font and dungeon shapes are programmatic placeholders. PNG asset loading and sprite animations will be added when art is ready.
