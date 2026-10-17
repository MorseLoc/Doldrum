// FEATURE LIBRARY: F011 Menus
// Full behavior/dependencies: docs/FEATURE_LIBRARY.md; metadata: core/FeatureIndex.hpp
#include "core/Game.hpp"
// ui/PixelFont.cpp: single-purpose feature implementation. See docs/FEATURE_LIBRARY.md.
namespace doldrum_game {
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
void text(SDL_Renderer* ren,std::string_view s,float x,float y,float scale,Uint8 r,Uint8 g,Uint8 b,Uint8 a) {
    for (char c : s) {
        const auto rows=glyph(c);
        for(int yy=0;yy<7;++yy) for(int xx=0;xx<5;++xx)
            if(rows[static_cast<size_t>(yy)] & (1u << (4-xx)))
                rect(ren,x+xx*scale,y+yy*scale,scale,scale,r,g,b,a);
        x+=6*scale;
    }
}
void centered(SDL_Renderer* ren,std::string_view s,float y,float scale,Uint8 r,Uint8 g,Uint8 b,Uint8 a) {
    text(ren,s,(W-textWidth(s,scale))*0.5f,y,scale,r,g,b,a);
}

} // namespace doldrum_game
