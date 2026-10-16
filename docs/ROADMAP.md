# Doldrum Design / Roadmap 0.1

## Game vision
Angled 2D pixel-art roguelike with projectile-based combat and hybrid procedural dungeon made from authored room templates. In-run coins can be deposited at bank rooms; dying loses unbanked coins. Winning extracts carried coins. Hub currency buys upgrades and cards. Themed decks unlock progressively, cards are purchased once, remain permanently active, and can introduce positive/negative new mechanics, enemies, areas, and interactions. Special boss cards lead to other decks/areas. Item effects should generally apply across characters, including weapon-like melee attacks treated as attack instances. The game rewards learning compound rule interactions. Card and item encyclopedia required.

## Implementation stages
1. Window/input/fixed loop/pixel scaling (Build 001)
2. TXT tile room format, coordinate grid, collisions, doors, player/attacks
3. One enemy, damage, one room-clear encounter
4. Procedural room connections, traversal, floors
5. Persistent profile, banks, hub, run vs meta economy
6. Declarative card effects, themed deck draw/purchase, encyclopedia
7. First boss progression gate and second deck

## Constraints
- Keep cards and content addressable by stable string IDs.
- Use versioned saves and reserve support for migrations.
- World geometry and logical coordinates separate from artwork.
- Ensure attack effect recursion and spawning are bounded.
- Purchased cards may alter earlier cards' behavior.
- Don't allow empty decks or boss RNG to deadlock progression.

## Build 001 scope
Visual placeholder only. Movement, arrows firing, SDL3 input, 60 Hz fixed update. No collisions except arena bounds and bullet despawn.
