#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"
#include "char_select.h"

int char_select() {
    bool menuAtivo = true;
    bool keyArrowUp = false, keyArrowDown = false, keyArrowLeft = false, keyArrowRight = false, keyEnter = false;

    int i;
    int posX = 10, posY = 10, habilityIndex = 0;
    int currentCharacter = 0;

    SDL_Event eventos;
    SDL_Rect select = {6, 6, 158 , 233};
    SDL_Rect hab = {550, 50, 400, 624};
    SDL_Rect chars[9];

    for (i = 0; i < 9; i++) {
        chars[i].x = posX;
        chars[i].y = posY;
        chars[i].w = 150;
        chars[i].h = 225;

        if ((i+1)%3 == 0) {
            posX = 10;
            posY += 229;
        } else {
            posX += 154;
        }
    }

    // Imagens do Menu
    SDL_Surface *default_char, *bacon, *hability;

    SDL_Surface *characterPortrait[9];
    characterPortrait[0] = SDL_LoadBMP("imgs/papaleguas.bmp");
    characterPortrait[1] = SDL_LoadBMP("imgs/reagan.bmp");
    characterPortrait[2] = SDL_LoadBMP("imgs/vini.bmp");
    characterPortrait[3] = SDL_LoadBMP("imgs/scorpion.bmp");
    characterPortrait[4] = SDL_LoadBMP("imgs/tati.bmp");
    characterPortrait[5] = SDL_LoadBMP("imgs/wander.bmp");
    characterPortrait[6] = SDL_LoadBMP("imgs/zeca.bmp");
    characterPortrait[7] = SDL_LoadBMP("imgs/frozen.bmp");
    characterPortrait[8] = SDL_LoadBMP("imgs/tales.bmp");

    SDL_Surface *skills[9];
    skills[0] = SDL_LoadBMP("imgs/papaleguas_hab.bmp");
    skills[1] = SDL_LoadBMP("imgs/reagan_hab.bmp");
    skills[2] = SDL_LoadBMP("imgs/vini_hab.bmp");
    skills[3] = SDL_LoadBMP("imgs/scorpion_hab.bmp");
    skills[4] = SDL_LoadBMP("imgs/tati_hab.bmp");
    skills[5] = SDL_LoadBMP("imgs/wander_hab.bmp");
    skills[6] = SDL_LoadBMP("imgs/zeca_hab.bmp");
    skills[7] = SDL_LoadBMP("imgs/elsa_hab.bmp");
    skills[8] = SDL_LoadBMP("imgs/tales_hab.bmp");

    hability = SDL_LoadBMP("imgs/default_hab.bmp");
    bacon = SDL_LoadBMP("imgs/bacon.bmp");

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
            if (currentCharacter > 2) currentCharacter -= 3;
            keyArrowUp = false;
        }

        if (keyArrowDown) {
            if (currentCharacter < 6) currentCharacter += 3;
            keyArrowDown = false;
        }

        if (keyArrowLeft) {
            if (currentCharacter > 0) currentCharacter--;
            keyArrowLeft = false;
        }

        if (keyArrowRight) {
            if (currentCharacter < 8) currentCharacter ++;
            keyArrowRight = false;
        }

        if (keyEnter) {
            return currentCharacter;
        }

        select.x = chars[currentCharacter].x - 4;
        select.y = chars[currentCharacter].y - 4;

        SDL_BlitSurface(bacon, NULL, game.screenSurface, NULL);
        SDL_BlitSurface(skills[currentCharacter], NULL, game.screenSurface, &hab);

        SDL_FillRect(game.screenSurface, &select, 0x5EFE18);

        for (i = 0; i < 9; i++) {
            SDL_BlitSurface(characterPortrait[i], NULL, game.screenSurface, &chars[i]);
        }

        SDL_UpdateWindowSurface(game.window);
        SDL_Delay(50);
    }

    return -1;
}
