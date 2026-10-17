# Doldrum Build 008

Based on the previously supplied Build 007, not GitHub (which currently has Build 005).
Copy this folder's contents over your existing repository, keeping `.git` and PNG art.

## Changes
- F015: Squibs drop coin/bomb/key/half-heart with 5% chance **each** (20% total).
- F016: small HUD coin/bomb/key symbols stacked directly below HP.
- F017: player collides with banks; touch from outside auto-deposits coins.
- F020: boss prevents player passage while alive, still deals contact damage.
- F022: hub Tutorial Deck stand, purchase once for one banked coin with E. No card effects yet.
- F023: optional two-frame walking sprites plus two transparent shooting overlays.
- Save slots migrate v1/v2 -> v3 with purchased Tutorial Deck flag.

## Character animation filenames
Add to `assets/characters/`:
`DoldWalk1.png`, `DoldWalk2.png`, `DoldFire1.png`, `DoldFire2.png`.
Existing `Dold.png` remains idle fallback. See `assets/characters/ANIMATION_GUIDE.txt`.

## Important
Animation frames preserve the full PNG canvas and draw on the same 16x16 space.
Keep all four animation files the same dimensions and alignment.
The legacy idle Dold.png continues to use alpha-cropping.

## Test plan
1. Launch a save, touch hub bank from outside; check coins deposit and can't walk through it.
2. Earn bank coins, approach the hub DECK stand and press E; it costs 1 coin only once.
3. Restart save slot; purchase is retained. v1/v2 saves should remain readable.
4. Verify HUD counters appear in 3 rows below the health bar.
5. Fight Squibs to confirm all four types can appear (20% chance per kill).
6. In boss room confirm player cannot pass through boss. Boss bullets still hurt.
7. Add animation PNGs and test walking and shooting simultaneously.

This was not compiled or executed in Visual Studio/Windows here.
