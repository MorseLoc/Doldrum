#pragma once
// FEATURE LIBRARY: F009 Health, F014 Application
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp

// Doldrum gameplay library: definitions are shared but behaviors live in separate modules.
// Dependencies: SDL3 types, Room spatial data, STL only. Avoid SDL window ownership here.
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "world/Room.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace doldrum_game {
constexpr int W = 320, H = 192; // viewport, NOT world dimensions
constexpr float STEP=1.f/60.f, PLAYER_SPEED=94.f, BULLET_SPEED=185.f, FIRE_DELAY=.58f;
constexpr float ENEMY_SPEED=34.f;
constexpr int PLAYER_MAX_HP=6;
constexpr float BODY_HALF=5.0f;
constexpr int DUNGEON_START=1;
enum class Screen { Title, Saves, Hub, Dungeon, Paused, GameOver };
struct Squib { SDL_FPoint pos{}; int hp = 2; };
struct Sprite { SDL_Texture* texture=nullptr; SDL_FRect source{}; };
struct Art { Sprite floor,wall,door,dold,squib,shot; };
struct Bullet { SDL_FPoint pos{}; SDL_FPoint vel{}; };
struct SaveSlot { bool exists = false; std::uint64_t seconds = 0; };
struct Button { std::string text; SDL_FRect bounds{}; float glow = 0; bool enabled = true; };
struct Game {
    std::array<doldrum::Room, 3> rooms;
    SDL_FPoint player{};
    SDL_FPoint camera{};
    std::vector<Bullet> bullets;
    std::array<std::vector<Squib>,3> enemies{};
    std::array<bool,3> roomsCleared{};
    int hp = PLAYER_MAX_HP;
    float invulnerable = 0.0f;
    Art art{};
    int menuSelection = 0;
    int roomIndex = 0;
    int activeSlot = -1;
    Screen screen = Screen::Title;
    Screen pausedFrom = Screen::Hub;
    std::array<SaveSlot,3> saves{};
    std::filesystem::path saveDir;
    std::vector<Button> buttons;
    bool deleteMode = false;
    int deleteChoice = -1;
    bool running = true;
    float shootCooldown = 0;
    float menuClock = 0;
    float playSubsecond = 0;
    float autosaveClock = 0;
    bool fullScreen = false;
};


// Public interfaces for each feature; implementation documented in .cpp files.
void rect(SDL_Renderer*,float,float,float,float,Uint8,Uint8,Uint8,Uint8=255);
std::array<std::uint8_t,7> glyph(char);
float textWidth(std::string_view,float);
void text(SDL_Renderer*,std::string_view,float,float,float,Uint8,Uint8,Uint8,Uint8=255);
void centered(SDL_Renderer*,std::string_view,float,float,Uint8,Uint8,Uint8,Uint8=255);
std::string timeString(std::uint64_t);
std::filesystem::path slotPath(const Game&,int);
void loadSlots(Game&);
void saveActive(Game&);
void resetToHub(Game&);
void toTitle(Game&);
void openSlot(Game&,int);
bool canOccupy(const doldrum::Room&,float,float);
bool solidAt(const doldrum::Room&,float,float);
void spawnSquibs(Game&,int);
void changeRoom(Game&,int,float,float);
void movePlayer(Game&,const bool*,float); // Player.cpp: movement and walls
void fireAndAdvanceShots(Game&,const bool*,float); // Projectile.cpp
void updateSquibs(Game&,float); // Squib.cpp
void updateWorld(Game&,const bool*,float); // WorldSystem.cpp: orchestrates

void addButton(Game&,std::string,float,float,float,float,bool=true);
void rebuildButtons(Game&);
int hoveredButton(const Game&,float,float);
void click(Game&,int);
void update(Game&,const bool*,float,float,float);
void menuBackground(SDL_Renderer*,float);
void drawButtons(SDL_Renderer*,const Game&);
Sprite loadSprite(SDL_Renderer*,const std::filesystem::path&);
void loadArt(Game&,SDL_Renderer*,const std::filesystem::path&);
void freeArt(Art&);
void sprite(SDL_Renderer*,const Sprite&,float,float,float,float);
void entity(SDL_Renderer*,const Sprite&,float,float,float,float);
void world(SDL_Renderer*,const Game&);
void render(SDL_Renderer*,const Game&);
void updateCamera(Game&);
} // namespace doldrum_game
