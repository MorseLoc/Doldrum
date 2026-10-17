#include "core/Game.hpp"
#include "core/FeatureIndex.hpp"
#include <SDL3/SDL_main.h>
// Entry point owns SDL, routes events, and runs the fixed-timestep simulation.
int main(int,char**) {
    using namespace doldrum_game;
    if(!SDL_Init(SDL_INIT_VIDEO)){std::fprintf(stderr,"SDL_Init failed: %s\n",SDL_GetError());return 1;}
    SDL_Window* window=nullptr;SDL_Renderer* renderer=nullptr;
    if(!SDL_CreateWindowAndRenderer("Doldrum - Build 008",960,576,SDL_WINDOW_RESIZABLE,&window,&renderer)){
        std::fprintf(stderr,"Window creation failed: %s\n",SDL_GetError());SDL_Quit();return 1;
    }
    SDL_SetRenderLogicalPresentation(renderer,W,H,SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(renderer,1);
    Game g;
    try {
        const auto root=std::filesystem::path(SDL_GetBasePath())/"data"/"rooms";
        g.rooms[0]=doldrum::loadRoom((root/"room_hub.txt").string());
        g.rooms[1]=doldrum::loadRoom((root/"room_start.txt").string());
        g.rooms[2]=doldrum::loadRoom((root/"room_large.txt").string());
        g.rooms[3]=doldrum::loadRoom((root/"room_boss.txt").string());
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
                if(event.key.scancode==SDL_SCANCODE_E && g.screen==Screen::Hub){
                    interactShop(g);
                }else if(event.key.scancode==SDL_SCANCODE_F){
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
