// FEATURE LIBRARY: F008 Projectile
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp
#include "core/Game.hpp"
// PROJECTILE FEATURE: starter four-way firing, cooldown, movement and wall impacts.
// Enemy-hit resolution is currently owned by Squib; future effect pipelines will
// process hit events independently of each enemy type.
namespace doldrum_game {
void fireAndAdvanceShots(Game& g,const bool* keys,float dt){
    const auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    g.shootCooldown=std::max(0.0f,g.shootCooldown-dt);
    SDL_FPoint dir{};
    if(keys[SDL_SCANCODE_UP])dir={0,-1};
    else if(keys[SDL_SCANCODE_DOWN])dir={0,1};
    else if(keys[SDL_SCANCODE_LEFT])dir={-1,0};
    else if(keys[SDL_SCANCODE_RIGHT])dir={1,0};
    if((dir.x!=0||dir.y!=0)&&g.shootCooldown<=0) {
        g.bullets.push_back({g.player,{dir.x*BULLET_SPEED,dir.y*BULLET_SPEED}});
        g.shootCooldown=FIRE_DELAY;
    }
    for(auto& b:g.bullets){b.pos.x+=b.vel.x*dt;b.pos.y+=b.vel.y*dt;}
    std::erase_if(g.bullets,[&](const Bullet& b){return solidAt(room,b.pos.x,b.pos.y);});
}
} // namespace doldrum_game
