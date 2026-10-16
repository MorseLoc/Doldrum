#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "world/Room.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr int kLogicalWidth = doldrum::kRoomWidth * doldrum::kTileSize;
constexpr int kLogicalHeight = doldrum::kRoomHeight * doldrum::kTileSize;
constexpr double kFixedStep = 1.0 / 60.0;
constexpr float kPlayerSpeed = 94.0f;
constexpr float kBulletSpeed = 185.0f;
constexpr float kShotCooldown = 0.18f;
constexpr float kHalfBody = 4.0f;

struct Bullet { SDL_FPoint position{}; SDL_FPoint velocity{}; };

struct Game {
    std::array<doldrum::Room, 2> rooms;
    int currentRoom = 0;
    SDL_FPoint player{160.0f, 96.0f};
    std::vector<Bullet> bullets;
    float shotTimer = 0.0f;
    bool running = true;
};

void fillRect(SDL_Renderer* renderer, float x, float y, float w, float h,
              Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    const SDL_FRect rect{x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

bool canOccupy(const doldrum::Room& room, float x, float y) {
    // Keep the player's feet clear of solid tiles. Doorway border cells are passable.
    for (float ox : {-kHalfBody, kHalfBody}) for (float oy : {-kHalfBody, kHalfBody}) {
        const int tx = static_cast<int>(std::floor((x + ox) / doldrum::kTileSize));
        const int ty = static_cast<int>(std::floor((y + oy) / doldrum::kTileSize));
        if (room.isSolid(tx, ty)) return false;
    }
    return true;
}

bool isWallAt(const doldrum::Room& room, float x, float y) {
    return room.isSolid(static_cast<int>(std::floor(x / doldrum::kTileSize)),
                        static_cast<int>(std::floor(y / doldrum::kTileSize)));
}

void changeRoom(Game& game, int next, float x, float y) {
    game.currentRoom = next;
    game.player = {x, y};
    game.bullets.clear();
}

void update(Game& game, const bool* keys, float dt) {
    auto& room = game.rooms[static_cast<size_t>(game.currentRoom)];
    float dx = static_cast<float>(keys[SDL_SCANCODE_D]) - static_cast<float>(keys[SDL_SCANCODE_A]);
    float dy = static_cast<float>(keys[SDL_SCANCODE_S]) - static_cast<float>(keys[SDL_SCANCODE_W]);
    const float magnitude = std::sqrt(dx * dx + dy * dy);
    if (magnitude > 0.0f) { dx /= magnitude; dy /= magnitude; }
    const float nx = game.player.x + dx * kPlayerSpeed * dt;
    const float ny = game.player.y + dy * kPlayerSpeed * dt;
    if (canOccupy(room, nx, game.player.y)) game.player.x = nx;
    if (canOccupy(room, game.player.x, ny)) game.player.y = ny;

    // Two hand-authored connected rooms. Full procedural assembly comes in Build 004.
    const int doorY = static_cast<int>(game.player.y / doldrum::kTileSize);
    if (game.currentRoom == 0 && game.player.x >= kLogicalWidth - 13 &&
        room.isDoor(doldrum::kRoomWidth - 1, doorY)) {
        changeRoom(game, 1, 21.0f, game.player.y);
        return;
    }
    if (game.currentRoom == 1 && game.player.x <= 13 && room.isDoor(0, doorY)) {
        changeRoom(game, 0, kLogicalWidth - 21.0f, game.player.y);
        return;
    }

    game.shotTimer = std::max(0.0f, game.shotTimer - dt);
    SDL_FPoint direction{};
    if (keys[SDL_SCANCODE_UP]) direction = {0.0f, -1.0f};
    else if (keys[SDL_SCANCODE_DOWN]) direction = {0.0f, 1.0f};
    else if (keys[SDL_SCANCODE_LEFT]) direction = {-1.0f, 0.0f};
    else if (keys[SDL_SCANCODE_RIGHT]) direction = {1.0f, 0.0f};
    if ((direction.x != 0.0f || direction.y != 0.0f) && game.shotTimer <= 0.0f) {
        game.bullets.push_back({game.player,
            {direction.x * kBulletSpeed, direction.y * kBulletSpeed}});
        game.shotTimer = kShotCooldown;
    }
    for (Bullet& bullet : game.bullets) {
        bullet.position.x += bullet.velocity.x * dt;
        bullet.position.y += bullet.velocity.y * dt;
    }
    std::erase_if(game.bullets, [&](const Bullet& bullet) {
        return isWallAt(room, bullet.position.x, bullet.position.y);
    });
}

void render(SDL_Renderer* renderer, const Game& game) {
    SDL_SetRenderDrawColor(renderer, 14, 18, 28, 255);
    SDL_RenderClear(renderer);
    const auto& room = game.rooms[static_cast<size_t>(game.currentRoom)];
    for (int y = 0; y < doldrum::kRoomHeight; ++y) {
        for (int x = 0; x < doldrum::kRoomWidth; ++x) {
            const int px = x * doldrum::kTileSize, py = y * doldrum::kTileSize;
            const auto& tile = room.tileAt(x, y);
            if (tile == "A0") {
                fillRect(renderer, static_cast<float>(px), static_cast<float>(py), 16, 16,
                         74, 78, 93);
                fillRect(renderer, static_cast<float>(px + 1), static_cast<float>(py + 1), 14, 4,
                         103, 108, 124);
            } else if (tile == "D0") {
                fillRect(renderer, static_cast<float>(px), static_cast<float>(py), 16, 16,
                         118, 82, 49);
                fillRect(renderer, static_cast<float>(px + 4), static_cast<float>(py + 4), 8, 8,
                         167, 125, 67);
            } else {
                const bool alt = (x + y) % 2 == 0;
                fillRect(renderer, static_cast<float>(px), static_cast<float>(py), 16, 16,
                         alt ? 42 : 46, alt ? 46 : 50, alt ? 57 : 61);
            }
        }
    }
    // Temporary character sprite, grounded to logical floor coordinates.
    fillRect(renderer, game.player.x - 5, game.player.y + 4, 10, 3, 23, 25, 33);
    fillRect(renderer, game.player.x - 5, game.player.y - 7, 10, 12, 103, 189, 158);
    fillRect(renderer, game.player.x - 3, game.player.y - 10, 6, 4, 229, 209, 168);
    for (const Bullet& bullet : game.bullets)
        fillRect(renderer, bullet.position.x - 2, bullet.position.y - 2, 4, 4, 246, 194, 105);
    SDL_RenderPresent(renderer);
}
} // namespace

int main(int, char**) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("Doldrum - Build 002", 960, 576,
                                     SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        std::fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderLogicalPresentation(renderer, kLogicalWidth, kLogicalHeight,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(renderer, 1);

    Game game;
    try {
        const std::filesystem::path root = std::filesystem::path(SDL_GetBasePath()) / "data" / "rooms";
        game.rooms[0] = doldrum::loadRoom((root / "room_start.txt").string());
        game.rooms[1] = doldrum::loadRoom((root / "room_east.txt").string());
        game.player = {(game.rooms[0].spawn.x + 0.5f) * doldrum::kTileSize,
                       (game.rooms[0].spawn.y + 0.5f) * doldrum::kTileSize};
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "Room loading failed: %s\n", ex.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Doldrum room error", ex.what(), window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Uint64 previous = SDL_GetPerformanceCounter();
    const Uint64 frequency = SDL_GetPerformanceFrequency();
    double accumulator = 0.0;
    while (game.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.scancode == SDL_SCANCODE_ESCAPE)) game.running = false;
        }
        const Uint64 now = SDL_GetPerformanceCounter();
        const double elapsed = std::min(0.25, static_cast<double>(now - previous) /
                                             static_cast<double>(frequency));
        previous = now;
        accumulator += elapsed;
        const bool* keys = SDL_GetKeyboardState(nullptr);
        while (accumulator >= kFixedStep) {
            update(game, keys, static_cast<float>(kFixedStep));
            accumulator -= kFixedStep;
        }
        render(renderer, game);
        SDL_Delay(1);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
