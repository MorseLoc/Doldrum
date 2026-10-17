#include "core/Game.hpp"
namespace doldrum_game {
void rect(SDL_Renderer* ren, float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren,r,g,b,a);
    SDL_FRect dst{x,y,w,h}; SDL_RenderFillRect(ren,&dst);
}


}
