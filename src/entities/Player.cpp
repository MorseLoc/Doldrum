// FEATURE LIBRARY: F001 Player
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp
#include "core/Game.hpp"
// PLAYER FEATURE: 16x16 visual sprite, axis-separated movement and a smaller
// collision body for forgiving Zelda-style tile traversal. Depends only on Room.
namespace doldrum_game {
bool canOccupy(const doldrum::Room& room,float x,float y) {
    for(float ox:{-BODY_HALF,BODY_HALF})for(float oy:{-BODY_HALF,BODY_HALF}) {
        const int tx=static_cast<int>(std::floor((x+ox)/doldrum::kTileSize));
        const int ty=static_cast<int>(std::floor((y+oy)/doldrum::kTileSize));
        if(room.isSolid(tx,ty))return false;
    }
    return true;
}
bool solidAt(const doldrum::Room& room,float x,float y) {
    return room.isSolid(static_cast<int>(std::floor(x/doldrum::kTileSize)),
                        static_cast<int>(std::floor(y/doldrum::kTileSize)));
}
void movePlayer(Game& g,const bool* keys,float dt) {
    const auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    float dx=static_cast<float>(keys[SDL_SCANCODE_D])-static_cast<float>(keys[SDL_SCANCODE_A]);
    float dy=static_cast<float>(keys[SDL_SCANCODE_S])-static_cast<float>(keys[SDL_SCANCODE_W]);
    const float len=std::sqrt(dx*dx+dy*dy);
    if(len>0){dx/=len;dy/=len;}
    const float nx=g.player.x+dx*PLAYER_SPEED*dt;
    const float ny=g.player.y+dy*PLAYER_SPEED*dt;
    g.walking=len>0.f;
    if(g.walking)g.walkingClock+=dt;
    else g.walkingClock=0.f;
    if(canOccupy(room,nx,g.player.y)&&!bankBlocks(g,g.roomIndex,nx,g.player.y)&&!bossBlocks(g,nx,g.player.y))g.player.x=nx;
    if(canOccupy(room,g.player.x,ny)&&!bankBlocks(g,g.roomIndex,g.player.x,ny)&&!bossBlocks(g,g.player.x,ny))g.player.y=ny;
}
} // namespace doldrum_game
