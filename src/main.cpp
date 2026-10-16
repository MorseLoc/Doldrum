#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "world/Room.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
constexpr int W = doldrum::kRoomWidth * doldrum::kTileSize;
constexpr int H = doldrum::kRoomHeight * doldrum::kTileSize;
constexpr float STEP = 1.0f / 60.0f;
constexpr float PLAYER_SPEED = 94.0f;
constexpr float BULLET_SPEED = 185.0f;
constexpr float FIRE_DELAY = .58f;
constexpr float ENEMY_SPEED = 34.0f;
constexpr int PLAYER_MAX_HP = 6;
constexpr float BODY_HALF = 4.0f;
constexpr int DUNGEON_START = 1;

enum class Screen { Title, Saves, Hub, Dungeon, Paused, GameOver };
struct Squib { SDL_FPoint pos{}; int hp = 2; };
struct Sprite { SDL_Texture* texture=nullptr; SDL_FRect source{}; };
struct Art { Sprite floor,wall,door,dold,squib,shot; };
struct Bullet { SDL_FPoint pos{}; SDL_FPoint vel{}; };
struct SaveSlot { bool exists = false; std::uint64_t seconds = 0; };
struct Button { std::string text; SDL_FRect bounds{}; float glow = 0; bool enabled = true; };
struct Game {
    std::array<doldrum::Room, 3> rooms;
    SDL_FPoint player{};
    std::vector<Bullet> bullets;
    std::array<std::vector<Squib>,3> enemies{};
    std::array<bool,3> roomsCleared{};
    int hp = PLAYER_MAX_HP;
    float invulnerable = 0.0f;
    Art art{};
    int menuSelection = 0;
    int roomIndex = 0;
    int activeSlot = -1;
    Screen screen = Screen::Title;
    Screen pausedFrom = Screen::Hub;
    std::array<SaveSlot,3> saves{};
    std::filesystem::path saveDir;
    std::vector<Button> buttons;
    bool deleteMode = false;
    int deleteChoice = -1;
    bool running = true;
    float shootCooldown = 0;
    float menuClock = 0;
    float playSubsecond = 0;
    float autosaveClock = 0;
    bool fullScreen = false;
};

void rect(SDL_Renderer* ren, float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a=255) {
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren,r,g,b,a);
    SDL_FRect dst{x,y,w,h}; SDL_RenderFillRect(ren,&dst);
}

// Tiny built-in 5x7 pixel alphabet, deliberately independent of external font files.
std::array<std::uint8_t,7> glyph(char c) {
    switch(c) {
    case 'A': return {14,17,17,31,17,17,17}; case 'B': return {30,17,17,30,17,17,30};
    case 'C': return {14,17,16,16,16,17,14}; case 'D': return {30,17,17,17,17,17,30};
    case 'E': return {31,16,16,30,16,16,31}; case 'F': return {31,16,16,30,16,16,16};
    case 'G': return {14,17,16,23,17,17,15}; case 'H': return {17,17,17,31,17,17,17};
    case 'I': return {31,4,4,4,4,4,31}; case 'J': return {7,2,2,2,18,18,12};
    case 'K': return {17,18,20,24,20,18,17}; case 'L': return {16,16,16,16,16,16,31};
    case 'M': return {17,27,21,21,17,17,17}; case 'N': return {17,25,21,19,17,17,17};
    case 'O': return {14,17,17,17,17,17,14}; case 'P': return {30,17,17,30,16,16,16};
    case 'Q': return {14,17,17,17,21,18,13}; case 'R': return {30,17,17,30,20,18,17};
    case 'S': return {15,16,16,14,1,1,30}; case 'T': return {31,4,4,4,4,4,4};
    case 'U': return {17,17,17,17,17,17,14}; case 'V': return {17,17,17,17,17,10,4};
    case 'W': return {17,17,17,21,21,21,10}; case 'X': return {17,17,10,4,10,17,17};
    case 'Y': return {17,17,10,4,4,4,4}; case 'Z': return {31,1,2,4,8,16,31};
    case '0': return {14,17,19,21,25,17,14}; case '1': return {4,12,4,4,4,4,14};
    case '2': return {14,17,1,2,4,8,31}; case '3': return {30,1,1,14,1,1,30};
    case '4': return {2,6,10,18,31,2,2}; case '5': return {31,16,30,1,1,17,14};
    case '6': return {7,8,16,30,17,17,14}; case '7': return {31,1,2,4,8,8,8};
    case '8': return {14,17,17,14,17,17,14}; case '9': return {14,17,17,15,1,2,28};
    case ':': return {0,4,4,0,4,4,0}; case '-': return {0,0,0,31,0,0,0};
    case '/': return {1,1,2,4,8,16,16}; case '<': return {2,4,8,16,8,4,2};
    case '>': return {8,4,2,1,2,4,8}; case '.': return {0,0,0,0,0,12,12};
    case '?': return {14,17,1,2,4,0,4}; case '!': return {4,4,4,4,4,0,4};
    case '+': return {0,4,4,31,4,4,0}; case ' ': return {};
    default: return {};
    }
}
float textWidth(std::string_view s,float scale) { return static_cast<float>(s.size()) * 6.0f * scale; }
void text(SDL_Renderer* ren,std::string_view s,float x,float y,float scale,Uint8 r,Uint8 g,Uint8 b,Uint8 a=255) {
    for (char c : s) {
        const auto rows=glyph(c);
        for(int yy=0;yy<7;++yy) for(int xx=0;xx<5;++xx)
            if(rows[static_cast<size_t>(yy)] & (1u << (4-xx)))
                rect(ren,x+xx*scale,y+yy*scale,scale,scale,r,g,b,a);
        x+=6*scale;
    }
}
void centered(SDL_Renderer* ren,std::string_view s,float y,float scale,Uint8 r,Uint8 g,Uint8 b,Uint8 a=255) {
    text(ren,s,(W-textWidth(s,scale))*0.5f,y,scale,r,g,b,a);
}
std::string timeString(std::uint64_t seconds) {
    const auto hrs=seconds/3600; const auto mins=(seconds/60)%60;
    std::ostringstream out;out<<std::setfill('0')<<std::setw(2)<<hrs<<":"<<std::setw(2)<<mins;return out.str();
}
std::filesystem::path slotPath(const Game& g,int slot) {
    return g.saveDir / ("slot_"+std::to_string(slot+1)+".txt");
}
void loadSlots(Game& g) {
    for(int i=0;i<3;++i) {
        g.saves[static_cast<size_t>(i)]={};
        std::ifstream in(slotPath(g,i));
        int version=0;std::uint64_t secs=0;
        if(in && (in>>version>>secs) && version==1) g.saves[static_cast<size_t>(i)]={true,secs};
    }
}
void saveActive(Game& g) {
    if(g.activeSlot<0) return;
    std::error_code error;
    std::filesystem::create_directories(g.saveDir,error);
    std::ofstream out(slotPath(g,g.activeSlot),std::ios::trunc);
    if(out) out<<1<<" "<<g.saves[static_cast<size_t>(g.activeSlot)].seconds<<"\n";
    else std::fprintf(stderr,"Could not save slot %d\n",g.activeSlot+1);
}
void resetToHub(Game& g) {
    g.roomIndex=0; g.screen=Screen::Hub;
    const auto& sp=g.rooms[0].spawn;
    g.player={(sp.x+.5f)*doldrum::kTileSize,(sp.y+.5f)*doldrum::kTileSize};
    g.bullets.clear();g.shootCooldown=0;g.playSubsecond=0;g.autosaveClock=0;
    g.hp=PLAYER_MAX_HP;g.invulnerable=0;g.roomsCleared.fill(false);
    for(auto& enemies:g.enemies)enemies.clear();
}
void toTitle(Game& g) {
    saveActive(g);g.activeSlot=-1;g.screen=Screen::Title;
    g.deleteMode=false;g.deleteChoice=-1;g.bullets.clear();g.menuSelection=0;
}
void openSlot(Game& g,int slot) {
    g.activeSlot=slot;
    if(!g.saves[static_cast<size_t>(slot)].exists) {
        g.saves[static_cast<size_t>(slot)]={true,0};saveActive(g);
    }
    resetToHub(g);
}

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
}
void updateWorld(Game& g,const bool* keys,float dt) {
    auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    float dx=static_cast<float>(keys[SDL_SCANCODE_D])-static_cast<float>(keys[SDL_SCANCODE_A]);
    float dy=static_cast<float>(keys[SDL_SCANCODE_S])-static_cast<float>(keys[SDL_SCANCODE_W]);
    const float len=std::sqrt(dx*dx+dy*dy);
    if(len>0){dx/=len;dy/=len;}
    const float nx=g.player.x+dx*PLAYER_SPEED*dt;
    const float ny=g.player.y+dy*PLAYER_SPEED*dt;
    if(canOccupy(room,nx,g.player.y))g.player.x=nx;
    if(canOccupy(room,g.player.x,ny))g.player.y=ny;
    const int xTile=static_cast<int>(g.player.x/doldrum::kTileSize);
    const int yTile=static_cast<int>(g.player.y/doldrum::kTileSize);
    if(g.roomIndex==0 && g.player.y<=13 && room.isDoor(xTile,0)) {
        g.screen=Screen::Dungeon;changeRoom(g,DUNGEON_START,160,96);return;
    }
    if(g.roomIndex==1 && g.player.x>=W-13 && room.isDoor(doldrum::kRoomWidth-1,yTile)) {
        changeRoom(g,2,21,g.player.y);return;
    }
    if(g.roomIndex==2 && g.player.x<=13 && room.isDoor(0,yTile)) {
        changeRoom(g,1,W-21,g.player.y);return;
    }
    g.invulnerable=std::max(0.0f,g.invulnerable-dt);
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
void addButton(Game& g,std::string label,float x,float y,float w,float h,bool enabled=true) {
    g.buttons.push_back({std::move(label),{x,y,w,h},0,enabled});
}
void rebuildButtons(Game& g) {
    // Keep hover animation state for buttons with the same label.
    const auto previous=g.buttons;
    g.buttons.clear();
    if(g.screen==Screen::Title) {
        addButton(g,"PLAY GAME",70,104,180,23);
        addButton(g,"QUIT",70,135,180,23);
    } else if(g.screen==Screen::Saves) {
        for(int i=0;i<3;++i) addButton(g,"SLOT "+std::to_string(i+1),40,40+i*37,240,31,
                                        !g.deleteMode||g.saves[static_cast<size_t>(i)].exists);
        addButton(g,"< BACK",34,165,113,19);
        addButton(g,g.deleteChoice>=0?"CONFIRM DELETE":g.deleteMode?"CANCEL DELETE":"DELETE FILE",151,165,148,19);
    } else if(g.screen==Screen::GameOver) {
        addButton(g,"RETURN TO HUB",50,110,220,25);
    } else if(g.screen==Screen::Paused) {
        addButton(g,"CONTINUE",67,86,186,24);
        addButton(g,"QUIT TO MENU",67,123,186,24);
    }
    g.menuSelection=std::clamp(g.menuSelection,0,std::max(0,static_cast<int>(g.buttons.size())-1));
    for(auto& b:g.buttons)for(const auto& old:previous)
        if(old.text==b.text) {b.glow=old.glow;break;}
}
int hoveredButton(const Game& g,float x,float y) {
    for(size_t i=0;i<g.buttons.size();++i) {
        const auto& b=g.buttons[i];
        if(b.enabled && x>=b.bounds.x && y>=b.bounds.y&&x<b.bounds.x+b.bounds.w&&y<b.bounds.y+b.bounds.h)
            return static_cast<int>(i);
    }
    return -1;
}
void click(Game& g,int id) {
    if(id<0 || id>=static_cast<int>(g.buttons.size()) || !g.buttons[static_cast<size_t>(id)].enabled)return;
    if(g.screen==Screen::GameOver) {
        resetToHub(g);
    } else if(g.screen==Screen::Title) {
        if(id==0) {loadSlots(g);g.screen=Screen::Saves;}
        else if(id==1)g.running=false;
    } else if(g.screen==Screen::Saves) {
        if(id>=0&&id<3) {
            if(g.deleteMode){g.deleteChoice=id;}
            else openSlot(g,id);
        } else if(id==3) {
            if(g.deleteMode){g.deleteMode=false;g.deleteChoice=-1;}
            else g.screen=Screen::Title;
        } else if(id==4) {
            if(g.deleteChoice>=0){
                std::error_code error;
                std::filesystem::remove(slotPath(g,g.deleteChoice),error);
                if(error)std::fprintf(stderr,"Delete failed: %s\n",error.message().c_str());
                loadSlots(g);g.deleteChoice=-1;g.deleteMode=false;
            } else {g.deleteMode=!g.deleteMode;g.deleteChoice=-1;}
        }
    } else if(g.screen==Screen::Paused) {
        if(id==0)g.screen=g.pausedFrom;
        else if(id==1)toTitle(g);
    }
    g.menuSelection=0;rebuildButtons(g);
}
void update(Game& g,const bool* keys,float dt,float mouseX,float mouseY) {
    g.menuClock+=dt;
    const int hover=hoveredButton(g,mouseX,mouseY);
    for(size_t i=0;i<g.buttons.size();++i) {
        auto& b=g.buttons[i];
        const float target=b.enabled && (hover==static_cast<int>(i)||g.menuSelection==static_cast<int>(i))?1.0f:0.0f;
        b.glow=std::clamp(b.glow+(target>b.glow?dt*6.0f:-dt*3.7f),0.0f,1.0f);
    }
    if(g.screen!=Screen::Hub&&g.screen!=Screen::Dungeon)return;
    updateWorld(g,keys,dt);
    if(g.activeSlot>=0) {
        g.playSubsecond+=dt;g.autosaveClock+=dt;
        if(g.playSubsecond>=1.0f){++g.saves[static_cast<size_t>(g.activeSlot)].seconds;g.playSubsecond-=1.0f;}
        if(g.autosaveClock>=10.0f){saveActive(g);g.autosaveClock=0;}
    }
}
void menuBackground(SDL_Renderer* ren,float t) {
    rect(ren,0,0,W,H,11,13,22);
    for(int i=0;i<20;++i) {
        const float phase=static_cast<float>(i)*.83f;
        const float brightness=(std::sin(t*.45f+phase)+1.0f)*.5f;
        const int x=(i*73+19)%W,y=(i*47+61)%H;
        rect(ren,static_cast<float>(x),static_cast<float>(y),2,2,117,122,149,
             static_cast<Uint8>(20+brightness*35));
    }
    rect(ren,0,H-19,W,19,26,28,43,110);
}
void drawButtons(SDL_Renderer* ren,const Game& g) {
    for(const auto& b:g.buttons){
        const float glow=b.enabled?b.glow:0;
        const Uint8 bg=static_cast<Uint8>(48+glow*65);
        rect(ren,b.bounds.x,b.bounds.y,b.bounds.w,b.bounds.h,bg,static_cast<Uint8>(bg+3),
             static_cast<Uint8>(bg+12),static_cast<Uint8>((b.enabled?65:25)+glow*85));
        if(glow>.01f){
            rect(ren,b.bounds.x,b.bounds.y,b.bounds.w,1,219,204,166,static_cast<Uint8>(glow*225));
            rect(ren,b.bounds.x,b.bounds.y+b.bounds.h-1,b.bounds.w,1,219,204,166,static_cast<Uint8>(glow*200));
        }
        const Uint8 intensity=static_cast<Uint8>((b.enabled?137:75)+glow*(b.enabled?111:0));
        const float sc=b.bounds.h>=23?1.65f:1.45f;
        const float tx=b.bounds.x+(b.bounds.w-textWidth(b.text,sc))*.5f;
        text(ren,b.text,tx,b.bounds.y+(b.bounds.h-7*sc)*.5f,sc,intensity,intensity,
             static_cast<Uint8>(std::min(255,static_cast<int>(intensity)+7)));
    }
}
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
    // Sprite feet are the simulation position; art extends above the floor.
    sprite(renderer,s,x,y+6,width,height);
}
void world(SDL_Renderer* ren,const Game& g) {
    const auto& room=g.rooms[static_cast<size_t>(g.roomIndex)];
    rect(ren,0,0,W,H,14,18,28);
    for(int y=0;y<doldrum::kRoomHeight;++y)for(int x=0;x<doldrum::kRoomWidth;++x) {
        const int px=x*doldrum::kTileSize,py=y*doldrum::kTileSize;
        const auto& tile=room.tileAt(x,y);
        if(tile=="A0") {
            if(g.art.wall.texture){sprite(ren,g.art.wall,static_cast<float>(px+8),static_cast<float>(py+16),16,20);continue;}
            rect(ren,static_cast<float>(px),static_cast<float>(py),16,16,74,78,93);
            rect(ren,static_cast<float>(px+1),static_cast<float>(py+1),14,4,103,108,124);
        }else if(tile=="D0") {
            if(g.art.door.texture){sprite(ren,g.art.door,static_cast<float>(px+8),static_cast<float>(py+16),16,19);continue;}
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
            rect(ren,g.player.x-5,g.player.y+4,10,3,23,25,33);
            if(g.art.dold.texture)entity(ren,g.art.dold,g.player.x,g.player.y,19,22);
            else{rect(ren,g.player.x-5,g.player.y-7,10,12,103,189,158);rect(ren,g.player.x-3,g.player.y-10,6,4,229,209,168);}
        }else{
            const auto& e=enemies[d.index];
            rect(ren,e.pos.x-5,e.pos.y+4,10,3,23,25,33);
            if(g.art.squib.texture)entity(ren,g.art.squib,e.pos.x,e.pos.y,18,18);
            else rect(ren,e.pos.x-6,e.pos.y-8,12,13,201,96,128);
        }
    }
    for(const auto& b:g.bullets){
        if(g.art.shot.texture)sprite(ren,g.art.shot,b.pos.x,b.pos.y+5,10,10);
        else rect(ren,b.pos.x-3,b.pos.y-3,6,6,246,194,105);
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
} // namespace

int main(int,char**) {
    if(!SDL_Init(SDL_INIT_VIDEO)){std::fprintf(stderr,"SDL_Init failed: %s\n",SDL_GetError());return 1;}
    SDL_Window* window=nullptr;SDL_Renderer* renderer=nullptr;
    if(!SDL_CreateWindowAndRenderer("Doldrum - Build 004",960,576,SDL_WINDOW_RESIZABLE,&window,&renderer)){
        std::fprintf(stderr,"Window creation failed: %s\n",SDL_GetError());SDL_Quit();return 1;
    }
    SDL_SetRenderLogicalPresentation(renderer,W,H,SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(renderer,1);
    Game g;
    try {
        const auto root=std::filesystem::path(SDL_GetBasePath())/"data"/"rooms";
        g.rooms[0]=doldrum::loadRoom((root/"room_hub.txt").string());
        g.rooms[1]=doldrum::loadRoom((root/"room_start.txt").string());
        g.rooms[2]=doldrum::loadRoom((root/"room_east.txt").string());
    }catch(const std::exception& ex){
        std::fprintf(stderr,"Room loading failed: %s\n",ex.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Doldrum room error",ex.what(),window);
        SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();return 1;
    }
    char* pref=SDL_GetPrefPath("MorseLoc","Doldrum");
    if(pref){g.saveDir=pref;SDL_free(pref);}else g.saveDir="saves";
    loadArt(g,renderer,std::filesystem::path(SDL_GetBasePath())/"assets");
    loadSlots(g);rebuildButtons(g);
    auto previous=SDL_GetPerformanceCounter();const auto freq=SDL_GetPerformanceFrequency();double accumulator=0;
    while(g.running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_EVENT_QUIT){saveActive(g);g.running=false;}
            if(event.type==SDL_EVENT_KEY_DOWN && !event.key.repeat){
                if(event.key.scancode==SDL_SCANCODE_F){
                    const bool desired=!g.fullScreen;
                    if(SDL_SetWindowFullscreen(window,desired))g.fullScreen=desired;
                }else if((g.screen==Screen::Title||g.screen==Screen::Saves||g.screen==Screen::Paused||g.screen==Screen::GameOver)&&
                         (event.key.scancode==SDL_SCANCODE_UP||event.key.scancode==SDL_SCANCODE_DOWN||
                          event.key.scancode==SDL_SCANCODE_LEFT||event.key.scancode==SDL_SCANCODE_RIGHT)){
                    const int delta=(event.key.scancode==SDL_SCANCODE_UP||event.key.scancode==SDL_SCANCODE_LEFT)?-1:1;
                    if(!g.buttons.empty()){
                        const int count=static_cast<int>(g.buttons.size());
                        for(int i=0;i<count;++i){g.menuSelection=(g.menuSelection+delta+count)%count;
                            if(g.buttons[static_cast<size_t>(g.menuSelection)].enabled)break;}
                    }
                }else if((event.key.scancode==SDL_SCANCODE_RETURN||event.key.scancode==SDL_SCANCODE_KP_ENTER)&&
                         (g.screen==Screen::Title||g.screen==Screen::Saves||g.screen==Screen::Paused||g.screen==Screen::GameOver)){
                    if(g.menuSelection>=0&&g.menuSelection<static_cast<int>(g.buttons.size())&&
                       g.buttons[static_cast<size_t>(g.menuSelection)].enabled)click(g,g.menuSelection);
                }else if(event.key.scancode==SDL_SCANCODE_ESCAPE){
                    if(g.screen==Screen::Hub||g.screen==Screen::Dungeon){
                        saveActive(g);g.pausedFrom=g.screen;g.screen=Screen::Paused;
                    }else if(g.screen==Screen::Paused){g.screen=g.pausedFrom;}
                    else if(g.screen==Screen::Saves){g.deleteMode=false;g.deleteChoice=-1;g.screen=Screen::Title;}
                    rebuildButtons(g);
                }
            }
            if(event.type==SDL_EVENT_MOUSE_MOTION&& !g.buttons.empty()){
                float mx=event.motion.x,my=event.motion.y;
                SDL_RenderCoordinatesFromWindow(renderer,mx,my,&mx,&my);
                const int idx=hoveredButton(g,mx,my);if(idx>=0)g.menuSelection=idx;
            }
            if(event.type==SDL_EVENT_MOUSE_BUTTON_DOWN&&event.button.button==SDL_BUTTON_LEFT){
                float mx=event.button.x,my=event.button.y;
                SDL_RenderCoordinatesFromWindow(renderer,mx,my,&mx,&my);
                click(g,hoveredButton(g,mx,my));
            }
        }
        const auto now=SDL_GetPerformanceCounter();
        const double elapsed=std::min(.25,static_cast<double>(now-previous)/static_cast<double>(freq));
        previous=now;accumulator+=elapsed;
        float mx=0,my=0;SDL_GetMouseState(&mx,&my);SDL_RenderCoordinatesFromWindow(renderer,mx,my,&mx,&my);
        const bool* keys=SDL_GetKeyboardState(nullptr);
        while(accumulator>=STEP){const Screen before=g.screen;update(g,keys,STEP,mx,my);if(g.screen!=before)rebuildButtons(g);accumulator-=STEP;}
        render(renderer,g);
        SDL_Delay(1);
    }
    saveActive(g);
    freeArt(g.art);SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();return 0;
}
