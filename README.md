# Doldrum Build 004: Combat + Art

Apply these changed files to your existing GitHub checkout. **Do not delete your Git repository or artwork.** The art PNGs live in your repo and are deliberately not re-bundled here.

## Controls
- WASD: movement
- Arrow keys: four-way fire in gameplay; navigate selected menu option in menus
- Enter: activate selected option
- Mouse: hover and click buttons
- F: toggle fullscreen
- Escape: pause or resume, or return from save menu

## Content
- Squibs enter dungeon rooms and pursue the player directly, stopping at walls. Three are placed using walkable tile coordinates. Two hits kill a Squib. Squibs do not respawn in cleared rooms during one run.
- Six maximum health points, HUD displays three heart-style meters, contact damage + invulnerability, death screen returning to hub for a new run.
- Projectile delay increased from 0.18 s to 0.58 s.
- PNG sprites loaded using SDL3_image, preserving transparency. Transparent-padding crops are calculated once at load time, so DoldShot.png's 32x32 transparent canvas does not shrink the shot. Dold/Squib sprite anchors align to ground with height overlapping floor tiles.
- Missing images use old placeholder drawings. Images are looked up by exact filenames in assets/ as committed to GitHub.

## Image filenames (preserve exact case)
- assets/backgrounds/Floor.png
- assets/tiles/Wall.png, Door.png
- assets/characters/Dold.png
- assets/enemies/Squib.png
- assets/projectiles/DoldShot.png

## Important
- SDL3_image is a new dependency, fetched by CMake on first configure if not already installed. An internet connection is needed on your Windows machine.
- Save slots retain playtime only, as before. Returning from death starts a new run, with the same save slot.
- PNGs and final art may require sizing adjustments after testing in the actual game.
- This build is not yet verified by a full Windows/SDL3 compile.
