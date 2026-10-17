#pragma once
// DOLDRUM FEATURE INDEX (stable IDs; update docs/FEATURE_LIBRARY.md when adding one)
// This file is metadata only. It does not own gameplay logic or register runtime behavior.
#include <array>
#include <string_view>

namespace doldrum_game::feature_library {
struct Feature {
    int id;
    std::string_view name;
    std::string_view implementation;
    std::string_view summary;
    std::string_view dependencies;
};
inline constexpr std::array<Feature, 23> catalog{{
    {1, "Player", "entities/Player.cpp", "WASD movement, normalized diagonal speed, room collisions, and tile-scale body.", "Room, Floor, Wall, Game"},
    {2, "Floor", "world/Tiles.hpp, world/Tiles.cpp, world/Room.cpp", "A1 walkable ground and eligible spawn surface.", "Room, Renderer"},
    {3, "Wall", "world/Tiles.hpp, world/Tiles.cpp, world/Room.cpp", "A0 solid tile blocks entities and projectiles.", "Room, Player, Squib, Projectile"},
    {4, "Door", "world/Tiles.hpp, world/WorldSystem.cpp", "D0 non-solid entry/exit; currently links rooms by explicit rules.", "Room, WorldSystem"},
    {5, "Room", "world/Room.hpp, world/Room.cpp", "Load variable-size TXT tile grids; validate dimensions and spawn; report walkable positions.", "Floor, Wall, Door"},
    {6, "Camera", "world/WorldSystem.cpp, render/Renderer.cpp", "Follow player within large rooms; clamp to room edges; leave HUD screen-anchored.", "Player, Room, Renderer"},
    {7, "Squib", "entities/Squib.cpp", "Eight HP, pathfinding pursuit and separation, contact damage.", "Player, Room, Projectile, Game"},
    {8, "Projectile", "combat/Projectile.cpp", "Four-direction shots, rate control, solid collision, enemy hits.", "Player, Room, Squib, Renderer"},
    {9, "Health", "entities/Squib.cpp, core/Game.hpp, render/Renderer.cpp", "Player HP, hit invulnerability, health HUD and death state.", "Squib, Game, UI"},
    {10, "Sprite Rendering", "render/Renderer.cpp, render/DrawUtils.cpp", "PNG loading, transparent cropping, entity and tile rendering.", "Room, Camera, Player, Squib, Projectile"},
    {11, "Menus", "ui/MenuSystem.cpp, ui/PixelFont.cpp, main.cpp", "Title, save slots, pause/death screens; mouse and arrows+Enter.", "Save System, Game"},
    {12, "Save System", "save/SaveSystem.cpp", "Three persistent slots, elapsed playtime and save-file deletion.", "Menus, Game"},
    {13, "World Flow", "world/WorldSystem.cpp", "Hub-to-dungeon transitions, room changes, encounter placement and update sequence.", "Room, Player, Camera, Squib, Projectile"},
    {14, "Application", "main.cpp, core/AppSystem.cpp, core/Game.hpp", "SDL lifetime, events, fixed-step updates and high-level screen state.", "All runtime features"},
    {15, "Loot", "world/Loot.cpp", "5 percent chance per Squib defeat; equal coin, bomb, key, half heart.", "Squib, Inventory"},
    {16, "Inventory HUD", "ui/InventoryHUD.cpp, render/Renderer.cpp", "Coin/bomb/key icons below health; bank totals only above bank props.", "Loot, Bank, Renderer"},
    {17, "Bank", "world/Bank.cpp, save/SaveSystem.cpp", "Touch box to auto-deposit carried coins; balance displayed on bank; per-slot persistence.", "Hub, Gauntlet, Save"},
    {18, "Gauntlet", "world/WorldSystem.cpp, world/Bank.cpp", "Nine Squibs in the large room; bank gated behind room clear.", "Squib, Bank"},
    {19, "Squib Navigation", "entities/SquibNavigation.cpp, entities/Squib.cpp", "Wall-routing BFS flow field and body separation.", "Room, Squib, Player"},
    {20, "Zigzag Boss", "entities/Boss.cpp", "64 HP 64x64 Squib-art boss; zigzag across room, summons 3 Squibs each 30 seconds, persists dead for run.", "Room, Squib, Projectile, Player"},
    {21, "Enemy Projectiles", "entities/Boss.cpp", "Boss fires a spread of aimed hostile shots; wall and player collision.", "Boss, Player, Room, Health"},
    {22, "Tutorial Deck Shop", "world/Shop.cpp, save/SaveSystem.cpp", "Hub shop sells permanent Tutorial Deck for one banked coin; contents reserved for future.", "Bank, Hub, Save"},
    {23, "Player Animation", "render/Renderer.cpp, entities/Player.cpp, combat/Projectile.cpp", "Two independent walk and firing-overlay frames; idle fallback PNG.", "Player, Projectile, Sprite Rendering"},
}};

constexpr const Feature* find(int id) {
    for (const auto& item : catalog) if (item.id == id) return &item;
    return nullptr;
}
} // namespace doldrum_game::feature_library
