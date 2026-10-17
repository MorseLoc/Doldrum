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
constexpr int BOSS_ROOM=3;
enum class Screen { Title, Saves, Hub, Dungeon, Paused, GameOver };
struct Squib { SDL_FPoint pos{}; int hp = 8; };
enum class LootType { Coin, Bomb, Key, HalfHeart };
struct Loot { SDL_FPoint pos{}; LootType type=LootType::Coin; };
struct Bank { SDL_FPoint pos{}; bool requiresClear=false; };
struct EnemyShot { SDL_FPoint pos{}; SDL_FPoint vel{}; };
struct Boss { SDL_FPoint pos{}; int hp=64; bool defeated=false; float phase=0.f; float fireTimer=1.1f; float summonTimer=30.f; };
struct Sprite { SDL_Texture* texture=nullptr; SDL_FRect source{}; };
struct Art { Sprite floor,wall,door,dold,squib,shot; std::array<Sprite,2> walkFrames{},fireOverlays{}; };
struct Bullet { SDL_FPoint pos{}; SDL_FPoint vel{}; };
struct SaveSlot { bool exists = false; std::uint64_t seconds = 0; int bankBalance=0; bool tutorialDeckOwned=false; };
struct Button { std::string text; SDL_FRect bounds{}; float glow = 0; bool enabled = true; };
struct Game {
    std::array<doldrum::Room, 4> rooms;
    SDL_FPoint player{};
    SDL_FPoint camera{};
    std::vector<Bullet> bullets;
    std::array<std::vector<Squib>,4> enemies{};
    std::array<bool,4> roomsCleared{};
    std::array<std::vector<Loot>,4> loot{};
    std::array<Bank,4> banks{};
    Boss boss{};
    std::vector<EnemyShot> enemyShots;
    int coins=0, bombs=0, keys=0;
    float walkingClock=0.f, firingClock=0.f;
    bool walking=false;
    SDL_FPoint lastFiringDirection{0.f,1.f};
    SDL_FPoint shopPos{};
    bool tutorialDeckOwned=false;
    float shopMessageTimer=0.f;
    std::string shopMessage;
    int bankBalance=0;
    float bankMessageTimer=0;
    std::string bankMessage;
    std::uint32_t rngState=0x8173a25bu;
    std::vector<int> navigation;
    int navigationRoom=-1, navigationTarget=-1;
    float navigationCooldown=0;
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
void updateLoot(Game&,float); // Loot.cpp, F015
void spawnLoot(Game&,SDL_FPoint); // Loot.cpp, F015
void setupBanks(Game&); // Bank.cpp F017
void interactBank(Game&); // Bank.cpp F017
void autoDepositBank(Game&); // Bank.cpp F017
void resetBoss(Game&); // Boss.cpp F020
void updateBoss(Game&,float); // Boss.cpp F020
void drawInventoryHUD(SDL_Renderer*,const Game&); // HUD.cpp F016
void updateNavigation(Game&,float); // SquibNavigation.cpp F019
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
bool bankBlocks(const Game&,int,float,float,float=BODY_HALF);
bool bossBlocks(const Game&,float,float,float=BODY_HALF);
void setupShop(Game&);
void interactShop(Game&);
} // namespace doldrum_game
