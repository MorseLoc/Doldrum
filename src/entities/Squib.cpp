// FEATURE LIBRARY: F007 Squib, F009 Health, F018 Gauntlet, F019 Navigation
#include "core/Game.hpp"
namespace doldrum_game {
namespace {
bool clearAt(const Game& g, const std::vector<Squib>& enemies, size_t self, float x,float y) {
    if(!canOccupy(g.rooms[static_cast<size_t>(g.roomIndex)],x,y))return false;
    constexpr float minDistance=13.0f;
    for(size_t j=0;j<enemies.size();++j){
        if(j==self||enemies[j].hp<=0)continue;
        float dx=x-enemies[j].pos.x,dy=y-enemies[j].pos.y;
        if(dx*dx+dy*dy<minDistance*minDistance)return false;
    }
    return true;
}
}
void updateSquibs(Game& g,float dt){
    if(g.roomIndex<=0)return;
    auto& enemies=g.enemies[static_cast<size_t>(g.roomIndex)];
    const auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    updateNavigation(g,dt);
    for(size_t i=0;i<enemies.size();++i){
        auto& enemy=enemies[i];
        SDL_FPoint target=g.player;
        const int tx=static_cast<int>(enemy.pos.x/doldrum::kTileSize);
        const int ty=static_cast<int>(enemy.pos.y/doldrum::kTileSize);
        if(room.inBounds(tx,ty) && g.navigation.size()==static_cast<size_t>(room.width*room.height)){
            const int here=g.navigation[static_cast<size_t>(ty*room.width+tx)];
            int best=here, bx=tx,by=ty;
            constexpr int ox[4]={0,1,0,-1},oy[4]={-1,0,1,0};
            for(int j=0;j<4;++j){
                const int nx=tx+ox[j],ny=ty+oy[j];
                if(!room.inBounds(nx,ny) || room.isSolid(nx,ny))continue;
                int v=g.navigation[static_cast<size_t>(ny*room.width+nx)];
                if(v>=0 && (best<0||v<best)){best=v;bx=nx;by=ny;}
            }
            if(bx!=tx||by!=ty) target={(bx+.5f)*doldrum::kTileSize,(by+.5f)*doldrum::kTileSize};
        }
        const float dx=target.x-enemy.pos.x,dy=target.y-enemy.pos.y;
        const float len=std::sqrt(dx*dx+dy*dy);
        if(len>.1f){
            const float step=std::min(ENEMY_SPEED*dt,len);
            const float vx=dx/len*step,vy=dy/len*step;
            // Axis-separated movement slides along walls and avoids other Squibs.
            if(clearAt(g,enemies,i,enemy.pos.x+vx,enemy.pos.y))enemy.pos.x+=vx;
            if(clearAt(g,enemies,i,enemy.pos.x,enemy.pos.y+vy))enemy.pos.y+=vy;
        }
        const float px=g.player.x-enemy.pos.x,py=g.player.y-enemy.pos.y;
        if(px*px+py*py<12.f*12.f && g.invulnerable<=0){
            --g.hp;g.invulnerable=1.0f;
            if(g.hp<=0){
                // Run items/currency are lost; deposited bank balance remains.
                g.coins=0;g.bombs=0;g.keys=0;
                g.screen=Screen::GameOver;g.menuSelection=0;g.bullets.clear();return;
            }
        }
    }
    std::erase_if(g.bullets,[&](const Bullet& b){
        for(auto& enemy:enemies){
            if(enemy.hp<=0)continue;
            const float x=b.pos.x-enemy.pos.x,y=b.pos.y-enemy.pos.y;
            if(x*x+y*y<10.f*10.f){--enemy.hp;return true;}
        }
        return false;
    });
    for(const auto& e:enemies)if(e.hp<=0)spawnLoot(g,e.pos);
    std::erase_if(enemies,[](const Squib& e){return e.hp<=0;});
    if(enemies.empty())g.roomsCleared[static_cast<size_t>(g.roomIndex)]=true;
}
} // namespace doldrum_game
