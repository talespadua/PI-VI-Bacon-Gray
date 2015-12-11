#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"
#include "char_select.h"

int level_select() {
    bool menuAtivo = true;
    bool keyArrowUp = false, keyArrowDown = false, keyArrowLeft = false, keyArrowRight = false, keyEnter = false;

    int i;
    int posX = 10, posY = 10;
    int currentSelection = 0;

    SDL_Event eventos;

    //Imagens do Menu
    SDL_Surface *defaultChar, *bacon, *hability;

    hability = SDL_LoadBMP("imgs/default_hab.bmp");
    bacon = SDL_LoadBMP("imgs/bacon.bmp");

    SDL_Surface *thumbs[6];
    thumbs[0] = SDL_LoadBMP("maps/map_thumb_0.bmp");
    thumbs[1] = SDL_LoadBMP("maps/map_thumb_1.bmp");
    thumbs[2] = SDL_LoadBMP("maps/map_thumb_2.bmp");
    thumbs[3] = SDL_LoadBMP("maps/map_thumb_3.bmp");
    thumbs[4] = SDL_LoadBMP("maps/map_thumb_4.bmp");
    thumbs[5] = SDL_LoadBMP("maps/map_thumb_5.bmp");

    SDL_Surface *levelDescription[6];
    levelDescription[0] = SDL_LoadBMP("maps/map_descript_0.bmp");
    levelDescription[1] = SDL_LoadBMP("maps/map_descript_1.bmp");
    levelDescription[2] = SDL_LoadBMP("maps/map_descript_2.bmp");
    levelDescription[3] = SDL_LoadBMP("maps/map_descript_3.bmp");
    levelDescription[4] = SDL_LoadBMP("maps/map_descript_4.bmp");
    levelDescription[5] = SDL_LoadBMP("maps/map_descript_5.bmp");

    SDL_Rect select = {6, 6, 235 , 233};
    SDL_Rect hab = {550, 50, 400, 624};
    SDL_Rect levels[9];

    for (i = 0; i < 6; i++) {
        levels[i].x = posX;
        levels[i].y = posY;
        levels[i].w = 227;
        levels[i].h = 225;

        if ((i+1)%2 == 0) {
            posX = 10;
            posY += 229;
        } else {
            posX += 231;
        }
    }

    /*FIM DA INICIALIZACAO*/

    while (menuAtivo) {
        while (SDL_PollEvent(&eventos)) {
            if (eventos.type == SDL_KEYDOWN) {
                if (eventos.key.keysym.sym == SDLK_DOWN) {
                    keyArrowDown = true;
                }

                if (eventos.key.keysym.sym == SDLK_UP) {
                    keyArrowUp = true;
                }

                if (eventos.key.keysym.sym == SDLK_LEFT) {
                    keyArrowLeft = true;
                }

                if (eventos.key.keysym.sym == SDLK_RIGHT){
                    keyArrowRight = true;
                }

                if (eventos.key.keysym.sym == SDLK_RETURN) {
                    keyEnter = true;
                }
            }

            if (eventos.type == SDL_KEYUP) {
                if (eventos.key.keysym.sym == SDLK_DOWN) {
                    keyArrowDown = false;
                }

                if (eventos.key.keysym.sym == SDLK_UP) {
                    keyArrowUp = false;
                }

                if (eventos.key.keysym.sym == SDLK_LEFT) {
                    keyArrowLeft = false;
                }

                if (eventos.key.keysym.sym == SDLK_RIGHT){
                    keyArrowRight = false;
                }

                if (eventos.key.keysym.sym == SDLK_RETURN) {
                    keyEnter = false;
                }
            }

            if (eventos.type == SDL_QUIT) {
                // TODO retornar código de erro
                menuAtivo = 0;
            }
        }

        if (keyArrowUp) {
            if (currentSelection > 1) currentSelection -= 2;
            keyArrowUp = false;
        }

        if (keyArrowDown) {
            if (currentSelection < 4) currentSelection +=2;
            keyArrowDown = false;
        }

        if (keyArrowLeft) {
            if (currentSelection > 0) currentSelection--;
            keyArrowLeft = false;
        }

        if (keyArrowRight) {
            if (currentSelection < 5) currentSelection++;
            keyArrowRight = false;
        }

        if (keyEnter) {
            return currentSelection + '0';
        }

        select.x = levels[currentSelection].x - 4;
        select.y = levels[currentSelection].y - 4;

        SDL_BlitSurface(bacon, NULL, game.screenSurface, NULL);
        SDL_BlitSurface(levelDescription[currentSelection], NULL, game.screenSurface, &hab);

        SDL_FillRect(game.screenSurface, &select, 0x5EFE18);

        for (i = 0; i < 6; i++) {
            SDL_BlitSurface(thumbs[i], NULL, game.screenSurface, &levels[i]);
        }

        SDL_UpdateWindowSurface(game.window);
        SDL_Delay(50);
    }

    return -1;
}
