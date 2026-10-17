// FEATURE LIBRARY: F016 Inventory HUD
// Three compact icon rows, strictly underneath HP at the upper-left.
#include "core/Game.hpp"
namespace doldrum_game {
void drawInventoryHUD(SDL_Renderer* ren,const Game& g){
    const float x=7.f;
    // Coin
    rect(ren,x,21,7,7,132,92,38); rect(ren,x+1,22,5,5,236,197,78);
    text(ren,std::to_string(g.coins),18,22,1.f,245,222,157);
    // Bomb
    rect(ren,x,34,7,7,43,48,60); rect(ren,x+2,32,2,3,248,179,81);
    rect(ren,x+2,35,2,2,126,134,151);
    text(ren,std::to_string(g.bombs),18,35,1.f,228,225,232);
    // Key
    rect(ren,x,47,4,4,227,190,99);rect(ren,x+1,48,2,2,43,45,49);
    rect(ren,x+4,49,5,2,227,190,99);rect(ren,x+7,51,2,2,227,190,99);
    text(ren,std::to_string(g.keys),18,48,1.f,236,217,163);
}
} // namespace doldrum_game
