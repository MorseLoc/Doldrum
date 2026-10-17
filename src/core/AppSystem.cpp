#include "core/Game.hpp"
// core/AppSystem.cpp: single-purpose feature implementation. See docs/FEATURE_LIBRARY.md.
namespace doldrum_game {
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

} // namespace doldrum_game
