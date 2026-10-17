// FEATURE LIBRARY: F011 Menus
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp
#include "core/Game.hpp"
// ui/MenuSystem.cpp: single-purpose feature implementation. See docs/FEATURE_LIBRARY.md.
namespace doldrum_game {
void addButton(Game& g,std::string label,float x,float y,float w,float h,bool enabled) {
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

} // namespace doldrum_game
