// FEATURE LIBRARY: F020 Boss, F021 Enemy Projectiles
// 64x64 Kracko-inspired Zigzag boss, 64HP, periodic aimed volleys and Squib summons.
#include "core/Game.hpp"
namespace doldrum_game {
namespace {
constexpr float BOSS_RADIUS=23.f;
constexpr float SHOT_SPEED=105.f;
void hurtPlayer(Game& g){
    if(g.invulnerable>0 || g.screen==Screen::GameOver)return;
    --g.hp;g.invulnerable=1.f;
    if(g.hp<=0){
        g.coins=g.bombs=g.keys=0;
        g.screen=Screen::GameOver;g.menuSelection=0;
        g.bullets.clear();
    }
}
void fireVolley(Game& g){
    const float dx=g.player.x-g.boss.pos.x,dy=g.player.y-g.boss.pos.y;
    const float angle=std::atan2(dy,dx);
    for(float offset : {-0.22f,0.f,0.22f}){
        const float a=angle+offset;
        g.enemyShots.push_back({g.boss.pos,{std::cos(a)*SHOT_SPEED,std::sin(a)*SHOT_SPEED}});
    }
}
void summonSquibs(Game& g){
    auto& enemies=g.enemies[BOSS_ROOM];
    if(enemies.size()>=12)return;
    const auto& room=g.rooms[BOSS_ROOM];
    constexpr float offsets[8][2]={{-48,0},{48,0},{0,-48},{0,48},{-48,-48},{48,-48},{-48,48},{48,48}};
    int added=0;
    for(const auto& offset:offsets){
        if(added>=3 || enemies.size()>=12)break;
        const float x=g.boss.pos.x+offset[0],y=g.boss.pos.y+offset[1];
        if(!canOccupy(room,x,y))continue;
        const float px=x-g.player.x,py=y-g.player.y;
        if(px*px+py*py<30.f*30.f)continue;
        bool occupied=false;
        for(const auto& e:enemies){float dx=e.pos.x-x,dy=e.pos.y-y;if(dx*dx+dy*dy<18.f*18.f){occupied=true;break;}}
        if(!occupied){enemies.push_back({{x,y},8});++added;}
    }
    // Enemy navigation recalculates around new bodies on the next update.
}
}
void resetBoss(Game& g){
    const auto& room=g.rooms[BOSS_ROOM];
    g.boss={};
    g.boss.pos={(room.width*.5f)*doldrum::kTileSize,4.5f*doldrum::kTileSize};
    g.enemyShots.clear();
}
void updateBoss(Game& g,float dt){
    if(g.roomIndex!=BOSS_ROOM || g.boss.defeated)return;
    auto& b=g.boss;
    const auto& room=g.rooms[BOSS_ROOM];
    // Zigzag in an upper and lower lane; triangle wave gives sharp direction changes.
    b.phase+=dt;
    const float t=std::fmod(b.phase*0.27f,2.f);
    const float zig=1.f-std::abs(t-1.f); // 0..1..0
    const float margin=3.8f*doldrum::kTileSize;
    const float left=margin,right=room.width*doldrum::kTileSize-margin;
    b.pos.x=left+(right-left)*zig;
    // Top and bottom sweep: smoothly transition between lanes while zigzagging.
    const float top=4.5f*doldrum::kTileSize;
    const float bottom=(room.height-4.5f)*doldrum::kTileSize;
    b.pos.y=top+(bottom-top)*(0.5f-0.5f*std::cos(b.phase*1.35f));
    // Boss collides with incoming player projectiles, not its own.
    std::erase_if(g.bullets,[&](const Bullet& shot){
        const float dx=shot.pos.x-b.pos.x,dy=shot.pos.y-b.pos.y;
        if(dx*dx+dy*dy>BOSS_RADIUS*BOSS_RADIUS)return false;
        --b.hp;
        return true;
    });
    if(b.hp<=0){
        b.hp=0;b.defeated=true;
        g.roomsCleared[BOSS_ROOM]=true;
        g.enemyShots.clear();
        return;
    }
    const float px=g.player.x-b.pos.x,py=g.player.y-b.pos.y;
    if(px*px+py*py<(BOSS_RADIUS+5)*(BOSS_RADIUS+5))hurtPlayer(g);
    if(g.screen==Screen::GameOver)return;
    b.fireTimer-=dt;
    if(b.fireTimer<=0.f){fireVolley(g);b.fireTimer+=1.7f;}
    b.summonTimer-=dt;
    if(b.summonTimer<=0.f){summonSquibs(g);b.summonTimer+=30.f;}
    for(auto& shot:g.enemyShots){shot.pos.x+=shot.vel.x*dt;shot.pos.y+=shot.vel.y*dt;}
    std::erase_if(g.enemyShots,[&](const EnemyShot& shot){
        const float dx=shot.pos.x-g.player.x,dy=shot.pos.y-g.player.y;
        if(dx*dx+dy*dy<8.f*8.f){hurtPlayer(g);return true;}
        return solidAt(room,shot.pos.x,shot.pos.y);
    });
    if(g.screen==Screen::GameOver)g.enemyShots.clear();
}
} // namespace doldrum_game

namespace doldrum_game {
bool bossBlocks(const Game& g,float x,float y,float half){
    if(g.roomIndex!=BOSS_ROOM || g.boss.defeated)return false;
    const float dx=x-g.boss.pos.x,dy=y-g.boss.pos.y;
    const float closestX=std::max(std::abs(dx)-half,0.f);
    const float closestY=std::max(std::abs(dy)-half,0.f);
    return closestX*closestX+closestY*closestY<26.f*26.f;
}
} // namespace doldrum_game
