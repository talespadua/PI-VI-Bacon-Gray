#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils.h"
#include "game.h"
#include "char_select.h"
#include "level_select.h"

void menu() {
    bool menuAtivo = true;
    bool keyArrowUp = false, keyArrowDown = false, keyEnter = false;

    int personagem, level;

    SDL_Event eventos;

    SDL_Rect iniRet = {410, 400, 163, 56};
    SDL_Rect sairRet = {410, 480, 163, 56};
    SDL_Rect select = {408, 398, 167, 60};
    // SDL_Rect name_ret = {0,0, 800,200};

    // Imagens do Menu
    SDL_Surface *iniciar, *sair, *name, *sasha_bacon;

    iniciar = SDL_LoadBMP("sprites/iniciar.bmp");
    sair = SDL_LoadBMP("sprites/sair.bmp");
    //name = SDL_LoadBMP("name.bmp");
    sasha_bacon = SDL_LoadBMP("imgs/sasha_bacon.bmp");

    /*FIM DA INICIALIZACAO*/

    while (menuAtivo) {
        while (SDL_PollEvent(&eventos)) {
            if (eventos.type == SDL_KEYDOWN) {
                if (eventos.key.keysym.sym == SDLK_UP) {
                    keyArrowUp = true;
                }

                if (eventos.key.keysym.sym == SDLK_DOWN) {
                    keyArrowDown = true;
                }

                if (eventos.key.keysym.sym == SDLK_RETURN) {
                    keyEnter = true;
                }
            }

            if (eventos.type == SDL_KEYUP) {
                if (eventos.key.keysym.sym == SDLK_UP){
                    keyArrowUp = false;
                }

                if (eventos.key.keysym.sym == SDLK_DOWN){
                    keyArrowDown = false;
                }

                if (eventos.key.keysym.sym == SDLK_RETURN){
                    keyEnter = false;
                }
            }

            if (eventos.type == SDL_QUIT) {
                // return código de erro
                menuAtivo = 0;
            }
        }

        if (keyArrowUp) {
            //mainloop();
            select.y = iniRet.y - 2;
            select.w = iniRet.w + 4;
        }

        if (keyArrowDown) {
            select.y = sairRet.y-2;
            select.w = sairRet.w + 4;
        }

        if (keyEnter) {
            if (select.y == iniRet.y - 2) {
                personagem = char_select();
                // level = level_select();
                return;
            }

            if (select.y == sairRet.y - 2) {
                exit(0);
            }
        }

        keyEnter = false;

        SDL_BlitSurface(sasha_bacon, NULL, game.screenSurface, NULL);
        SDL_FillRect(game.screenSurface, &select, 0x00FF00);
        SDL_BlitSurface(iniciar, NULL, game.screenSurface, &iniRet);
        SDL_BlitSurface(sair, NULL, game.screenSurface, &sairRet);
        //SDL_BlitSurface(name, NULL, game.screenSurface, &name_ret);

        SDL_UpdateWindowSurface(game.window);
        SDL_Delay(50);
    }
}
