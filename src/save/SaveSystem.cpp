// FEATURE LIBRARY: F012 Save System
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp
#include "core/Game.hpp"
// save/SaveSystem.cpp: single-purpose feature implementation. See docs/FEATURE_LIBRARY.md.
namespace doldrum_game {
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
        if(in && (in>>version>>secs) && (version==1||version==2||version==3)) {
            int balance=0;
            if(version>=2 && !(in>>balance)) continue;
            int owned=0;
            if(version>=3 && !(in>>owned)) continue;
            g.saves[static_cast<size_t>(i)]={true,secs,std::max(0,balance),owned!=0};
        }
    }
}
void saveActive(Game& g) {
    if(g.activeSlot<0) return;
    std::error_code error;
    std::filesystem::create_directories(g.saveDir,error);
    std::ofstream out(slotPath(g,g.activeSlot),std::ios::trunc);
    if(out) {
        auto& slot=g.saves[static_cast<size_t>(g.activeSlot)];
        slot.bankBalance=g.bankBalance;
        slot.tutorialDeckOwned=g.tutorialDeckOwned;
        out<<3<<" "<<slot.seconds<<" "<<slot.bankBalance<<" "<<(slot.tutorialDeckOwned?1:0)<<"\n";
    }
    else std::fprintf(stderr,"Could not save slot %d\n",g.activeSlot+1);
}
void resetToHub(Game& g) {
    g.roomIndex=0; g.screen=Screen::Hub;
    const auto& sp=g.rooms[0].spawn;
    g.player={(sp.x+.5f)*doldrum::kTileSize,(sp.y+.5f)*doldrum::kTileSize};
    g.bullets.clear();g.shootCooldown=0;g.playSubsecond=0;g.autosaveClock=0;
    g.hp=PLAYER_MAX_HP;g.invulnerable=0;g.roomsCleared.fill(false);
    g.coins=0;g.bombs=0;g.keys=0;g.bankMessage.clear();g.bankMessageTimer=0;
    g.navigation.clear();g.navigationRoom=-1;g.navigationTarget=-1;
    for(auto& loot:g.loot)loot.clear();
    setupBanks(g);
    setupShop(g);
    updateCamera(g);
    for(auto& enemies:g.enemies)enemies.clear();
    g.enemyShots.clear();
    resetBoss(g);
}
void toTitle(Game& g) {
    saveActive(g);g.activeSlot=-1;g.screen=Screen::Title;
    g.deleteMode=false;g.deleteChoice=-1;g.bullets.clear();g.menuSelection=0;
}
void openSlot(Game& g,int slot) {
    g.activeSlot=slot;
    g.bankBalance=g.saves[static_cast<size_t>(slot)].bankBalance;
    g.tutorialDeckOwned=g.saves[static_cast<size_t>(slot)].tutorialDeckOwned;
    if(!g.saves[static_cast<size_t>(slot)].exists) {
        g.saves[static_cast<size_t>(slot)]={true,0};saveActive(g);
    }
    g.bankBalance=g.saves[static_cast<size_t>(slot)].bankBalance;
    g.tutorialDeckOwned=g.saves[static_cast<size_t>(slot)].tutorialDeckOwned;
    resetToHub(g);
}


} // namespace doldrum_game
