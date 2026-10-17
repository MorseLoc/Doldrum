#include "core/Game.hpp"
// SQUIB FEATURE: two HP, straight-line pursuit, touch damage, existing collision.
// Squibs do not navigate around walls; enemy life persists until leaving the run.
namespace doldrum_game {
void updateSquibs(Game& g,float dt){
    const auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    if(g.roomIndex>0){
        auto& enemies=g.enemies[static_cast<size_t>(g.roomIndex)];
        for(auto& enemy:enemies){
            const float x=g.player.x-enemy.pos.x,y=g.player.y-enemy.pos.y;
            const float d=std::sqrt(x*x+y*y);
            if(d>0.001f){
                const float ex=enemy.pos.x+(x/d)*ENEMY_SPEED*dt;
                const float ey=enemy.pos.y+(y/d)*ENEMY_SPEED*dt;
                if(canOccupy(room,ex,enemy.pos.y))enemy.pos.x=ex;
                if(canOccupy(room,enemy.pos.x,ey))enemy.pos.y=ey;
            }
            if(d<11.0f&&g.invulnerable<=0){
                --g.hp;g.invulnerable=1.0f;
                if(g.hp<=0){g.screen=Screen::GameOver;g.menuSelection=0;g.bullets.clear();return;}
            }
        }
        std::erase_if(g.bullets,[&](const Bullet& b){
            for(auto& enemy:enemies){
                if(enemy.hp<=0)continue;
                const float x=b.pos.x-enemy.pos.x,y=b.pos.y-enemy.pos.y;
                if(x*x+y*y<9.0f*9.0f){--enemy.hp;return true;}
            }
            return false;
        });
        std::erase_if(enemies,[](const Squib& e){return e.hp<=0;});
        if(enemies.empty())g.roomsCleared[static_cast<size_t>(g.roomIndex)]=true;
    }
}
} // namespace doldrum_game
