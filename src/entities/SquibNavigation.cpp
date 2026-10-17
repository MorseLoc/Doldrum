// FEATURE LIBRARY: F019 Squib Navigation
// Breadth-first flow field around walls, recomputed when player changes tiles.
// Each Squib follows descending distances; blocked movement slides along walls.
#include "core/Game.hpp"
#include <queue>
namespace doldrum_game {
void updateNavigation(Game& g,float dt){
    const auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    const int tx=std::clamp(static_cast<int>(g.player.x/doldrum::kTileSize),0,room.width-1);
    const int ty=std::clamp(static_cast<int>(g.player.y/doldrum::kTileSize),0,room.height-1);
    const int target=ty*room.width+tx;
    g.navigationCooldown-=dt;
    if(g.navigationRoom==g.roomIndex && g.navigationTarget==target &&
       g.navigation.size()==static_cast<size_t>(room.width*room.height))return;
    if(g.navigationCooldown>0 && g.navigationRoom==g.roomIndex)return;
    g.navigationRoom=g.roomIndex;g.navigationTarget=target;g.navigationCooldown=.10f;
    g.navigation.assign(static_cast<size_t>(room.width*room.height),-1);
    if(room.isSolid(tx,ty))return;
    std::queue<int> q;q.push(target);g.navigation[static_cast<size_t>(target)]=0;
    constexpr int ox[4]={0,1,0,-1},oy[4]={-1,0,1,0};
    while(!q.empty()){
        const int pos=q.front();q.pop();
        const int x=pos%room.width,y=pos/room.width;
        for(int i=0;i<4;++i){
            const int nx=x+ox[i],ny=y+oy[i];
            if(!room.inBounds(nx,ny)||room.isSolid(nx,ny))continue;
            const int next=ny*room.width+nx;
            if(g.navigation[static_cast<size_t>(next)]!=-1)continue;
            g.navigation[static_cast<size_t>(next)]=g.navigation[static_cast<size_t>(pos)]+1;
            q.push(next);
        }
    }
}
} // namespace doldrum_game
