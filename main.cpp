#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <filesystem>
#include <vector>
#include "Chip8.hpp"


constexpr int LARGEUR_GRILLE = 64;
constexpr int HAUTEUR_GRILLE = 32;
constexpr int SCALE = 10;

int main(int argc, char* argv[]){

    int romIndex = 0;
    std::vector<std::string> roms;
    
    for (const auto& entry : std::filesystem::directory_iterator("./ROM")) {
        if((entry.is_regular_file()) && ((entry.path().extension() == ".ch8") || (entry.path().extension() == ".rom"))){
            roms.push_back(entry.path().filename().string());
        }
    }

    Chip8 c8{};

    enum class Etat {MENU,JEU};
    Etat etat = Etat::MENU;
    
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    SDL_Window* fenetre = SDL_CreateWindow("CHIP8 emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR_GRILLE * SCALE, HAUTEUR_GRILLE * SCALE, SDL_WINDOW_SHOWN);

    if (fenetre == nullptr) {
        std::cerr << "Erreur SDL_CreateWindow : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        std::cerr << "Erreur SDL_CreateRenderer : " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return 1;
    }

    TTF_Font* police = TTF_OpenFont("./8bit16.ttf",18);

    if (police == nullptr) {
        std::cerr << "Erreur chargement police : " << TTF_GetError() << std::endl;
        SDL_DestroyWindow(fenetre);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    SDL_Color couleurBlanche = {255, 255, 255, 255};
    SDL_Color couleurGrise = {100, 100, 100, 255};
    std::string texteAffiche;

    Uint32 dernierTempsTimer = SDL_GetTicks();
    bool enCours = true;
    SDL_Event event;
    int pixel;

    while (enCours) {
        if(etat == Etat::JEU){
            c8.emulateCycle();
            Uint32 tempsActuel = SDL_GetTicks();
            if (tempsActuel - dernierTempsTimer >= 16) {
                c8.decrementTimers();
                dernierTempsTimer = tempsActuel;
            }

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) enCours = false;
                else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_1:
                            c8.setKey(0,true);
                            break;
                        case SDL_SCANCODE_2:
                            c8.setKey(1,true);
                            break;
                        case SDL_SCANCODE_3:
                            c8.setKey(2,true);
                            break;
                        case SDL_SCANCODE_4:
                            c8.setKey(3,true);
                            break;
                    }
                    switch (event.key.keysym.sym) {
                        case SDLK_a:
                            c8.setKey(4,true);
                            break;
                        case SDLK_z:
                            c8.setKey(5,true);
                            break;
                        case SDLK_e:
                            c8.setKey(6,true);
                            break;
                        case SDLK_r:
                            c8.setKey(7,true);
                            break;
                        case SDLK_q:
                            c8.setKey(8,true);
                            break;
                        case SDLK_s:
                            c8.setKey(9,true);
                            break;
                        case SDLK_d:
                            c8.setKey(10,true);
                            break;
                        case SDLK_f:
                            c8.setKey(11,true);
                            break;
                        case SDLK_w:
                            c8.setKey(12,true);
                            break;
                        case SDLK_x:
                            c8.setKey(13,true);
                            break;
                        case SDLK_c:
                            c8.setKey(14,true);
                            break;
                        case SDLK_v:
                            c8.setKey(15,true);
                            break;
                        case SDLK_ESCAPE:
                            etat = Etat::MENU;
                    }
                }
                else if (event.type == SDL_KEYUP) {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_1:
                            c8.setKey(0,false);
                            break;
                        case SDL_SCANCODE_2:
                            c8.setKey(1,false);
                            break;
                        case SDL_SCANCODE_3:
                            c8.setKey(2,false);
                            break;
                        case SDL_SCANCODE_4:
                            c8.setKey(3,false);
                            break;
                    }
                    switch (event.key.keysym.sym) {
                        case SDLK_a:
                            c8.setKey(4,false);
                            break;
                        case SDLK_z:
                            c8.setKey(5,false);
                            break;
                        case SDLK_e:
                            c8.setKey(6,false);
                            break;
                        case SDLK_r:
                            c8.setKey(7,false);
                            break;
                        case SDLK_q:
                            c8.setKey(8,false);
                            break;
                        case SDLK_s:
                            c8.setKey(9,false);
                            break;
                        case SDLK_d:
                            c8.setKey(10,false);
                            break;
                        case SDLK_f:
                            c8.setKey(11,false);
                            break;
                        case SDLK_w:
                            c8.setKey(12,false);
                            break;
                        case SDLK_x:
                            c8.setKey(13,false);
                            break;
                        case SDLK_c:
                            c8.setKey(14,false);
                            break;
                        case SDLK_v:
                            c8.setKey(15,false);
                            break;
                    }
                }
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);


            for (int y = 0; y < HAUTEUR_GRILLE; ++y) {
                for (int x = 0; x < LARGEUR_GRILLE; ++x) {
                    pixel = c8.getEcran(x,y);
                    if (pixel == 1) {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    }
                    else{
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }
                    SDL_Rect pixel = { x * SCALE, y * SCALE, SCALE, SCALE };
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(1);
        }
        else{
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            int y_offset = 20; 

            for (int i = 0; i < roms.size(); ++i) {
                if (i == romIndex) {
                    texteAffiche = "> " + roms[i];
                } else {
                    texteAffiche = "  " + roms[i];
                }

                SDL_Color couleurTexte = (i == romIndex) ? couleurBlanche : couleurGrise;

                SDL_Surface* surfaceTexte = TTF_RenderText_Solid(police, texteAffiche.c_str(), couleurTexte);
                
                SDL_Texture* textureTexte = SDL_CreateTextureFromSurface(renderer, surfaceTexte);

                SDL_Rect positionTexte;
                positionTexte.x = 40;
                positionTexte.y = y_offset;
                positionTexte.w = surfaceTexte->w;
                positionTexte.h = surfaceTexte->h;

                SDL_RenderCopy(renderer, textureTexte, NULL, &positionTexte);

                SDL_FreeSurface(surfaceTexte);
                SDL_DestroyTexture(textureTexte);

                y_offset += 30; 
            }

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) enCours = false;
                else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_s:
                            if(romIndex < roms.size() -1){
                                romIndex++;
                            }
                            break;
                        case SDLK_z:
                            if(romIndex > 0){
                                romIndex--;
                            }
                            break;
                        case SDLK_RETURN: // Touche Entrée pour valider
                            if (!roms.empty()) { // Sécurité si le dossier est vide
                                c8.init();
                                // On reconstruit le chemin complet du fichier
                                std::string cheminFichier = "./ROM/" + roms[romIndex];
                                c8.loadROM(cheminFichier);
                                
                                // On change l'état pour lancer l'émulation
                                etat = Etat::JEU; 
                            }
                            break;
                    }
                }
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(1);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    TTF_CloseFont(police);
    TTF_Quit();
    SDL_Quit();

    return 0;
}