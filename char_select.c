#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "mainloop.h"
#include "char_select.h"

int char_select()
{
    bool menu_ativo = true, entra = false;
    typedef enum{UP, DOWN, LEFT, RIGHT};
    bool teclas[] = {false, false, false, false};

    int pos_x = 10, pos_y = 10;

    SDL_Rect select = {6, 6, 158 , 233};
    SDL_Rect hab = {550, 50, 400, 624};
    SDL_Rect chars[9];
    int i;
    for(i=0; i<9; i++)
    {
        chars[i].x = pos_x;
        chars[i].y = pos_y;
        chars[i].w = 150;
        chars[i].h = 225;

        if((i+1)%3 == 0){
            pos_x = 10;
            pos_y += 229;
        }
        else{
            pos_x += 154;
        }
    }

    SDL_Event eventos;

    //Imagens do Menu
    SDL_Surface *default_char, *bacon, *hability;

    default_char = SDL_LoadBMP("char.bmp");

    hability = SDL_LoadBMP("default_hab.bmp");
    bacon = SDL_LoadBMP("bacon.bmp");

    /*FIM DA INICIALIZACAO*/

    while(menu_ativo)
    {
        while(SDL_PollEvent(&eventos))
        {
            if(eventos.type == SDL_KEYDOWN)
            {
                if(eventos.key.keysym.sym == SDLK_DOWN){
                     teclas[DOWN] = true;
                }
                if(eventos.key.keysym.sym == SDLK_UP){
                      teclas[UP] = true;
                }
                if(eventos.key.keysym.sym == SDLK_LEFT){
                     teclas[LEFT] = true;
                }
                if(eventos.key.keysym.sym == SDLK_RIGHT){
                      teclas[RIGHT] = true;
                }
                if(eventos.key.keysym.sym == SDLK_RETURN){
                      entra = true;
                }
            }
            if(eventos.type == SDL_KEYUP)
            {
                if(eventos.key.keysym.sym == SDLK_DOWN){
                    teclas[DOWN] = false;
                }
                if(eventos.key.keysym.sym == SDLK_UP){
                    teclas[UP] = false;
                }
                if(eventos.key.keysym.sym == SDLK_LEFT){
                    teclas[LEFT] = false;
                }
                if(eventos.key.keysym.sym == SDLK_RIGHT){
                    teclas[RIGHT] = false;
                }
                if(eventos.key.keysym.sym == SDLK_RETURN){
                      entra = false;
                }
            }
            if(eventos.type == SDL_QUIT){
                menu_ativo = 0;
            }
        }

        if(teclas[UP]){
            if(select.y > 6){
                select.y -= 229;
                teclas[UP] = false;
            }
        }
        if(teclas[DOWN]){
            if(select.y < 464){
                select.y += 229;
                teclas[DOWN] = false;
            }
        }

        if(teclas[LEFT]){
            if(select.x > 6){
                select.x -= 154;
                teclas[LEFT] = false;
            }
        }
        if(teclas[RIGHT]){
            printf("%d", select.x);
            if(select.x < 314){
                select.x += 154;
                teclas[RIGHT] = false;
            }
        }
        if(entra){
            return 0;
        }
        entra = false;

        SDL_BlitSurface(bacon, NULL, game.screenSurface, NULL);
        SDL_BlitSurface(hability, NULL, game.screenSurface, &hab);

        SDL_FillRect(game.screenSurface, &select, 0x5EFE18);

        for(i=0;i<9;i++)
        {
            SDL_BlitSurface(default_char, NULL, game.screenSurface, &chars[i]);
        }

        SDL_UpdateWindowSurface(game.window);
        SDL_Delay(50);
    }
    //Destroi window
    //SDL_DestroyWindow( game.window );

    // desliga todos os subsistemas da SDL e libera recursos alocados a eles.
    // Esta função sempre deve ser chamada antes de você sair.
    //SDL_Quit();
    return -1;
}
