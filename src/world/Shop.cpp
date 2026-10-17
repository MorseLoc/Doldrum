// FEATURE LIBRARY: F022 Tutorial Deck Shop
// Hub-side one-time purchase. Banked coins are spent, not carried coins.
#include "core/Game.hpp"
namespace doldrum_game {
void setupShop(Game& g){
    const auto& room=g.rooms[0];
    const auto p=room.spawn;
    const auto cells=room.freeTiles({p});
    doldrum::GridPoint chosen{p.x-3,p.y};
    if(!room.inBounds(chosen.x,chosen.y) || room.isSolid(chosen.x,chosen.y)){
        if(!cells.empty()) chosen=cells.front();
    }
    g.shopPos={(chosen.x+.5f)*doldrum::kTileSize,(chosen.y+.5f)*doldrum::kTileSize};
}
void interactShop(Game& g){
    if(g.screen!=Screen::Hub)return;
    if(std::abs(g.player.x-g.shopPos.x)>24.f || std::abs(g.player.y-g.shopPos.y)>24.f)return;
    if(g.tutorialDeckOwned){g.shopMessage="TUTORIAL DECK OWNED";}
    else if(g.bankBalance<1){g.shopMessage="NEED 1 BANKED COIN";}
    else {
        --g.bankBalance;
        g.tutorialDeckOwned=true;
        g.shopMessage="TUTORIAL DECK UNLOCKED";
        saveActive(g);
    }
    g.shopMessageTimer=2.3f;
}
} // namespace doldrum_game
