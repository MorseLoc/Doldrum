// FEATURE LIBRARY: F017 Bank
// Automatically deposits carried coins when Dold collides with a bank.
#include "core/Game.hpp"
namespace doldrum_game {
void setupBanks(Game& g){
    const auto& h=g.rooms[0];
    g.banks[0]={ {(h.spawn.x+3.5f)*doldrum::kTileSize,(h.spawn.y+.5f)*doldrum::kTileSize},false};
    const auto& d=g.rooms[2];
    doldrum::GridPoint cell{d.width-4,d.height-4};
    if(d.isSolid(cell.x,cell.y)){
        const auto cells=d.freeTiles({d.spawn});
        if(!cells.empty())cell=cells.back();
    }
    g.banks[2]={{(cell.x+.5f)*doldrum::kTileSize,(cell.y+.5f)*doldrum::kTileSize},true};
}
void autoDepositBank(Game& g){
    if(g.coins<=0 || (g.screen!=Screen::Hub && g.screen!=Screen::Dungeon))return;
    if(g.roomIndex!=0 && g.roomIndex!=2)return;
    const auto& bank=g.banks[static_cast<size_t>(g.roomIndex)];
    if(bank.requiresClear && !g.roomsCleared[static_cast<size_t>(g.roomIndex)])return;
    const float dx=bank.pos.x-g.player.x,dy=bank.pos.y-g.player.y;
    // Touch from outside: box is solid and can no longer be walked through.
    if(std::abs(dx)>15.f || std::abs(dy)>14.f)return;
    const int amount=g.coins;
    g.bankBalance+=amount;
    g.coins=0;
    g.bankMessage="DEPOSITED "+std::to_string(amount);
    g.bankMessageTimer=1.8f;
    saveActive(g);
}
// Compatibility symbol for earlier code; auto-deposit is now called every tick.
void interactBank(Game& g){ autoDepositBank(g); }
} // namespace doldrum_game

namespace doldrum_game {
bool bankBlocks(const Game& g,int room,float x,float y,float half){
    if(room!=0 && room!=2)return false;
    const auto& b=g.banks[static_cast<size_t>(room)];
    return std::abs(x-b.pos.x)<9.f+half && std::abs(y-b.pos.y)<8.f+half;
}
} // namespace doldrum_game
