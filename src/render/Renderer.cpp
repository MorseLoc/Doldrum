#include "core/Game.hpp"
// render/Renderer.cpp: single-purpose feature implementation. See docs/FEATURE_LIBRARY.md.
namespace doldrum_game {
Sprite loadSprite(SDL_Renderer* renderer,const std::filesystem::path& path){
    Sprite sprite{};
    SDL_Surface* surface=IMG_Load(path.string().c_str());
    if(!surface){std::fprintf(stderr,"Optional art unavailable: %s (%s)\n",path.string().c_str(),SDL_GetError());return sprite;}
    int left=surface->w,top=surface->h,right=-1,bottom=-1;
    for(int y=0;y<surface->h;++y)for(int x=0;x<surface->w;++x){
        Uint8 r=0,g=0,b=0,a=0;
        if(SDL_ReadSurfacePixel(surface,x,y,&r,&g,&b,&a)&&a>0){
            left=std::min(left,x);top=std::min(top,y);right=std::max(right,x);bottom=std::max(bottom,y);
        }
    }
    if(right<left){SDL_DestroySurface(surface);return sprite;}
    sprite.source={static_cast<float>(left),static_cast<float>(top),static_cast<float>(right-left+1),static_cast<float>(bottom-top+1)};
    sprite.texture=SDL_CreateTextureFromSurface(renderer,surface);
    SDL_DestroySurface(surface);
    if(sprite.texture)SDL_SetTextureScaleMode(sprite.texture,SDL_SCALEMODE_NEAREST);
    return sprite;
}
void loadArt(Game& g,SDL_Renderer* renderer,const std::filesystem::path& root){
    g.art.floor=loadSprite(renderer,root/"backgrounds"/"Floor.png");
    g.art.wall=loadSprite(renderer,root/"tiles"/"Wall.png");
    g.art.door=loadSprite(renderer,root/"tiles"/"Door.png");
    g.art.dold=loadSprite(renderer,root/"characters"/"Dold.png");
    g.art.squib=loadSprite(renderer,root/"enemies"/"Squib.png");
    g.art.shot=loadSprite(renderer,root/"projectiles"/"DoldShot.png");
}
void freeArt(Art& art){for(auto* s:{&art.floor,&art.wall,&art.door,&art.dold,&art.squib,&art.shot}){if(s->texture)SDL_DestroyTexture(s->texture);s->texture=nullptr;}}
void sprite(SDL_Renderer* renderer,const Sprite& s,float cx,float bottom,float width,float height){
    if(!s.texture)return;
    const SDL_FRect dst{std::round(cx-width*.5f),std::round(bottom-height),width,height};
    SDL_RenderTexture(renderer,s.texture,&s.source,&dst);
}
void entity(SDL_Renderer* renderer,const Sprite& s,float x,float y,float width,float height){
    // Entity is centered in a top-down 16x16 tile, with transparent padding cropped.
    sprite(renderer,s,x,y+height*.5f,width,height);
}
void world(SDL_Renderer* ren,const Game& g) {
    const auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    rect(ren,0,0,W,H,14,18,28);
    for(int y=std::max(0,static_cast<int>(g.camera.y)/doldrum::kTileSize-1);y<std::min(room.height,(static_cast<int>(g.camera.y)+H)/doldrum::kTileSize+2);++y)for(int x=std::max(0,static_cast<int>(g.camera.x)/doldrum::kTileSize-1);x<std::min(room.width,(static_cast<int>(g.camera.x)+W)/doldrum::kTileSize+2);++x) {
        const int px=x*doldrum::kTileSize-static_cast<int>(g.camera.x),py=y*doldrum::kTileSize-static_cast<int>(g.camera.y);
        const auto& tile=room.tileAt(x,y);
        if(tile=="A0") {
            if(g.art.wall.texture){sprite(ren,g.art.wall,static_cast<float>(px+8),static_cast<float>(py+16),16,16);continue;}
            rect(ren,static_cast<float>(px),static_cast<float>(py),16,16,74,78,93);
            rect(ren,static_cast<float>(px+1),static_cast<float>(py+1),14,4,103,108,124);
        }else if(tile=="D0") {
            if(g.art.door.texture){sprite(ren,g.art.door,static_cast<float>(px+8),static_cast<float>(py+16),16,16);continue;}
            rect(ren,static_cast<float>(px),static_cast<float>(py),16,16,118,82,49);
            rect(ren,static_cast<float>(px+4),static_cast<float>(py+4),8,8,167,125,67);
        }else{
            if(g.art.floor.texture){sprite(ren,g.art.floor,static_cast<float>(px+8),static_cast<float>(py+16),16,16);continue;}
            const bool alt=(x+y)%2==0;
            rect(ren,static_cast<float>(px),static_cast<float>(py),16,16,
                 alt?42:46,alt?46:50,alt?57:61);
        }
    }
    if(g.roomIndex==0) {
        centered(ren,"HUB",30,1.8f,184,168,132);
        centered(ren,"NORTH DOOR TO DUNGEON",47,.9f,178,178,182);
    }
    struct DrawEntity{float y;int kind;size_t index;};
    std::vector<DrawEntity> drawable{{g.player.y,0,0}};
    const auto& enemies=g.enemies[static_cast<size_t>(g.roomIndex)];
    for(size_t i=0;i<enemies.size();++i)drawable.push_back({enemies[i].pos.y,1,i});
    std::sort(drawable.begin(),drawable.end(),[](const auto& a,const auto& b){return a.y<b.y;});
    for(const auto& d:drawable){
        if(d.kind==0){
            if(g.invulnerable>0 && static_cast<int>(g.invulnerable*12)%2==0)continue;
            rect(ren,g.player.x-g.camera.x-5,g.player.y-g.camera.y+4,10,3,23,25,33);
            if(g.art.dold.texture)entity(ren,g.art.dold,g.player.x-g.camera.x,g.player.y-g.camera.y,16,16);
            else{rect(ren,g.player.x-g.camera.x-5,g.player.y-g.camera.y-7,10,12,103,189,158);rect(ren,g.player.x-g.camera.x-3,g.player.y-g.camera.y-10,6,4,229,209,168);}
        }else{
            const auto& e=enemies[d.index];
            rect(ren,e.pos.x-g.camera.x-5,e.pos.y-g.camera.y+4,10,3,23,25,33);
            if(g.art.squib.texture)entity(ren,g.art.squib,e.pos.x-g.camera.x,e.pos.y-g.camera.y,16,16);
            else rect(ren,e.pos.x-g.camera.x-6,e.pos.y-g.camera.y-8,12,13,201,96,128);
        }
    }
    for(const auto& b:g.bullets){
        if(g.art.shot.texture)sprite(ren,g.art.shot,b.pos.x-g.camera.x,b.pos.y-g.camera.y+5,10,10);
        else rect(ren,b.pos.x-g.camera.x-3,b.pos.y-g.camera.y-3,6,6,246,194,105);
    }
    // Player health in half-heart increments; temporary rectangular HUD.
    for(int i=0;i<PLAYER_MAX_HP/2;++i){
        const int left=std::clamp(g.hp-2*i,0,2);
        rect(ren,5.0f+i*18,5,15,9,35,25,35,216);
        if(left>0)rect(ren,7.0f+i*18,7,static_cast<float>(left*5+1),5,221,71,93);
    }
}
void render(SDL_Renderer* ren,const Game& g) {
    SDL_SetRenderDrawColor(ren,11,13,22,255);SDL_RenderClear(ren);
    if(g.screen==Screen::Title){
        menuBackground(ren,g.menuClock);
        const float pulse=(std::sin(g.menuClock*1.1f)+1.0f)*.5f;
        centered(ren,"DOLDRUM",35,4.4f,198,183,154,static_cast<Uint8>(152+pulse*103));
        centered(ren,"A DUNGEON THAT REMEMBERS",77,1.05f,116,119,144);
        centered(ren,"F  FULLSCREEN",174,1.1f,114,117,129);
        drawButtons(ren,g);
    }else if(g.screen==Screen::Saves){
        menuBackground(ren,g.menuClock);
        centered(ren,g.deleteMode?"SELECT SAVE TO DELETE":"SELECT SAVE FILE",12,1.65f,202,188,166);
        drawButtons(ren,g);
        for(int i=0;i<3;++i){
            const auto& slot=g.saves[static_cast<size_t>(i)];
            text(ren,slot.exists?timeString(slot.seconds):"EMPTY",197,51+i*37,1.28f,
                 slot.exists?172:108,slot.exists?180:113,slot.exists?185:127);
            if(g.deleteChoice==i)text(ren,"!",46,51+i*37,1.4f,249,132,119);
        }
        if(g.deleteChoice>=0)centered(ren,"DELETE SELECTED SAVE?",151,1.0f,235,143,134);
    }else{
        world(ren,g);
        if(g.screen==Screen::GameOver){
            rect(ren,0,0,W,H,7,9,16,226);
            centered(ren,"YOU DIED",43,3.1f,235,126,137);
            drawButtons(ren,g);
        }
        if(g.screen==Screen::Paused){
            rect(ren,0,0,W,H,7,9,16,218);
            centered(ren,"PAUSED",43,3.1f,216,205,177);
            drawButtons(ren,g);
        }
    }
    SDL_RenderPresent(ren);
}

} // namespace doldrum_game
