// FEATURE LIBRARY: F015 Loot, F016 Inventory HUD
// F015: 20% to drop any pickup, 5% each for coin/bomb/key/half-heart.
#include "core/Game.hpp"
namespace doldrum_game {
namespace {
std::uint32_t nextRandom(Game& g){
    // Deterministic PRNG state (seed once per new run in a future update).
    g.rngState^=g.rngState<<13;g.rngState^=g.rngState>>17;g.rngState^=g.rngState<<5;
    return g.rngState;
}
}
void spawnLoot(Game& g,SDL_FPoint pos){
    const auto roll=nextRandom(g)%100;
    if(roll>=20)return;
    const auto type=static_cast<LootType>(roll/5);
    g.loot[static_cast<size_t>(g.roomIndex)].push_back({pos,type});
}
void updateLoot(Game& g,float){
    auto& drops=g.loot[static_cast<size_t>(g.roomIndex)];
    std::erase_if(drops,[&](const Loot& drop){
        const float dx=drop.pos.x-g.player.x,dy=drop.pos.y-g.player.y;
        if(dx*dx+dy*dy>12.f*12.f)return false;
        switch(drop.type){
            case LootType::Coin: ++g.coins;break;
            case LootType::Bomb: ++g.bombs;break;
            case LootType::Key: ++g.keys;break;
            case LootType::HalfHeart:g.hp=std::min(PLAYER_MAX_HP,g.hp+1);break;
        }
        return true;
    });
}
} // namespace doldrum_game
