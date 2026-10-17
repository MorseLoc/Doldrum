#include "core/Game.hpp"
// world/WorldSystem.cpp: single-purpose feature implementation. See docs/FEATURE_LIBRARY.md.
namespace doldrum_game {
void spawnSquibs(Game& g,int roomIndex) {
    if(roomIndex==0 || g.roomsCleared[static_cast<size_t>(roomIndex)] || !g.enemies[static_cast<size_t>(roomIndex)].empty())return;
    const auto& room=g.rooms[static_cast<size_t>(roomIndex)];
    const auto cells=room.freeTiles({room.spawn});
    // Select spread-out floor cells, never hardcoded screen coordinates.
    for(auto it=cells.rbegin();it!=cells.rend() && g.enemies[static_cast<size_t>(roomIndex)].size()<3;++it){
        const float px=(it->x+0.5f)*doldrum::kTileSize,py=(it->y+0.5f)*doldrum::kTileSize;
        const float dx=px-g.player.x,dy=py-g.player.y;
        if(dx*dx+dy*dy<55.0f*55.0f)continue;
        const auto& selected=g.enemies[static_cast<size_t>(roomIndex)];
        bool near=false;for(const auto& e:selected){const float xx=e.pos.x-px,yy=e.pos.y-py;if(xx*xx+yy*yy<48.0f*48.0f)near=true;}
        if(!near)g.enemies[static_cast<size_t>(roomIndex)].push_back({{px,py},2});
    }
}
void changeRoom(Game& g,int room,float x,float y) {
    g.roomIndex=room;g.player={x,y};g.bullets.clear();
    spawnSquibs(g,room);
    updateCamera(g);
}
void updateWorld(Game& g,const bool* keys,float dt) {
    auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    movePlayer(g,keys,dt);
    const int xTile=static_cast<int>(g.player.x/doldrum::kTileSize);
    const int yTile=static_cast<int>(g.player.y/doldrum::kTileSize);
    if(g.roomIndex==0 && g.player.y<=doldrum::kTileSize*.85f && room.isDoor(xTile,0)) {
        g.screen=Screen::Dungeon;changeRoom(g,DUNGEON_START,(g.rooms[1].spawn.x+.5f)*doldrum::kTileSize,(g.rooms[1].spawn.y+.5f)*doldrum::kTileSize);return;
    }
    if(g.roomIndex==1 && g.player.x>=room.width*doldrum::kTileSize-doldrum::kTileSize*.85f && room.isDoor(room.width-1,yTile)) {
        changeRoom(g,2,static_cast<float>(doldrum::kTileSize)+5,13.5f*doldrum::kTileSize);return;
    }
    if(g.roomIndex==2 && g.player.x<=doldrum::kTileSize*.85f && room.isDoor(0,yTile)) {
        changeRoom(g,1,g.rooms[1].width*doldrum::kTileSize-static_cast<float>(doldrum::kTileSize)-5,5.5f*doldrum::kTileSize);return;
    }
    g.invulnerable=std::max(0.0f,g.invulnerable-dt);
    fireAndAdvanceShots(g,keys,dt);
    updateSquibs(g,dt);
    updateCamera(g);
}
void updateCamera(Game& g) {
    const auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    const float maxX=std::max(0.f,static_cast<float>(room.width*doldrum::kTileSize-W));
    const float maxY=std::max(0.f,static_cast<float>(room.height*doldrum::kTileSize-H));
    g.camera.x=std::clamp(g.player.x-W*.5f,0.f,maxX);
    g.camera.y=std::clamp(g.player.y-H*.5f,0.f,maxY);
}
} // namespace doldrum_game
