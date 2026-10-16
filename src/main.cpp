#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <vector>

namespace {
constexpr int kLogicalWidth = 320;
constexpr int kLogicalHeight = 192;
constexpr double kFixedStep = 1.0 / 60.0;
constexpr float kPlayerSpeed = 94.0f;
constexpr float kBulletSpeed = 185.0f;
constexpr float kShotCooldown = 0.18f;

struct Bullet {
    SDL_FPoint position{};
    SDL_FPoint velocity{};
};

struct Game {
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

void update(Game& game, const bool* keys, float dt) {
    float dx = static_cast<float>(keys[SDL_SCANCODE_D]) -
               static_cast<float>(keys[SDL_SCANCODE_A]);
    float dy = static_cast<float>(keys[SDL_SCANCODE_S]) -
               static_cast<float>(keys[SDL_SCANCODE_W]);
    const float magnitude = std::sqrt(dx * dx + dy * dy);
    if (magnitude > 0.0f) {
        dx /= magnitude;
        dy /= magnitude;
    }
    game.player.x = std::clamp(game.player.x + dx * kPlayerSpeed * dt, 16.0f,
                               static_cast<float>(kLogicalWidth - 16));
    game.player.y = std::clamp(game.player.y + dy * kPlayerSpeed * dt, 16.0f,
                               static_cast<float>(kLogicalHeight - 16));

    game.shotTimer = std::max(0.0f, game.shotTimer - dt);
    // For the starting character, shooting is restricted to four directions.
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
    std::erase_if(game.bullets, [](const Bullet& bullet) {
        return bullet.position.x < 10 || bullet.position.x > kLogicalWidth - 10 ||
               bullet.position.y < 10 || bullet.position.y > kLogicalHeight - 10;
    });
}

void render(SDL_Renderer* renderer, const Game& game) {
    SDL_SetRenderDrawColor(renderer, 14, 18, 28, 255);
    SDL_RenderClear(renderer);
    // Temporary angled-dungeon palette and checker floor, not final art.
    for (int y = 12; y < kLogicalHeight - 12; y += 16) {
        for (int x = 12; x < kLogicalWidth - 12; x += 16) {
            const bool alt = ((x / 16) + (y / 16)) % 2 == 0;
            fillRect(renderer, static_cast<float>(x), static_cast<float>(y), 16, 16,
                     alt ? 42 : 46, alt ? 46 : 50, alt ? 57 : 61);
        }
    }
    fillRect(renderer, 0, 0, 320, 12, 78, 82, 96);
    fillRect(renderer, 0, 180, 320, 12, 78, 82, 96);
    fillRect(renderer, 0, 0, 12, 192, 78, 82, 96);
    fillRect(renderer, 308, 0, 12, 192, 78, 82, 96);

    // Placeholder shadow + body, positioned from feet (not a final sprite).
    fillRect(renderer, game.player.x - 5, game.player.y + 4, 10, 3, 23, 25, 33);
    fillRect(renderer, game.player.x - 5, game.player.y - 7, 10, 12, 103, 189, 158);
    fillRect(renderer, game.player.x - 3, game.player.y - 10, 6, 4, 229, 209, 168);
    for (const Bullet& bullet : game.bullets) {
        fillRect(renderer, bullet.position.x - 2, bullet.position.y - 2,
                 4, 4, 246, 194, 105);
    }
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
    if (!SDL_CreateWindowAndRenderer("Doldrum - Build 001", 960, 576,
                                     SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        std::fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderLogicalPresentation(renderer, kLogicalWidth, kLogicalHeight,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(renderer, 1);

    Game game;
    Uint64 previous = SDL_GetPerformanceCounter();
    const Uint64 frequency = SDL_GetPerformanceFrequency();
    double accumulator = 0.0;

    while (game.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN &&
                 event.key.scancode == SDL_SCANCODE_ESCAPE)) {
                game.running = false;
            }
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
