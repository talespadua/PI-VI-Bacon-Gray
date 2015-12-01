#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "mainloop.h"
#include "char_select.h"

int level_select()
{
    bool menu_ativo = true, entra = false;
    typedef enum{UP, DOWN, LEFT, RIGHT};
    bool teclas[] = {false, false, false, false};

    int pos_x = 10, pos_y = 10;
    int cur_select = 0;

    SDL_Surface *thumbs[6];
    thumbs[0] = SDL_LoadBMP("maps/map_thumb_0.bmp");
    thumbs[1] = SDL_LoadBMP("maps/map_thumb_1.bmp");
    thumbs[2] = SDL_LoadBMP("maps/map_thumb_2.bmp");
    thumbs[3] = SDL_LoadBMP("maps/map_thumb_3.bmp");
    thumbs[4] = SDL_LoadBMP("maps/map_thumb_4.bmp");
    thumbs[5] = SDL_LoadBMP("maps/map_thumb_5.bmp");

    SDL_Surface *lev_descript[6];
    lev_descript[0] = SDL_LoadBMP("maps/map_descript_0.bmp");
    lev_descript[1] = SDL_LoadBMP("maps/map_descript_1.bmp");
    lev_descript[2] = SDL_LoadBMP("maps/map_descript_2.bmp");
    lev_descript[3] = SDL_LoadBMP("maps/map_descript_3.bmp");
    lev_descript[4] = SDL_LoadBMP("maps/map_descript_4.bmp");
    lev_descript[5] = SDL_LoadBMP("maps/map_descript_5.bmp");


    SDL_Rect select = {6, 6, 235 , 233};
    SDL_Rect hab = {550, 50, 400, 624};
    SDL_Rect levels[9];
    int i;
    for(i=0; i<6; i++)
    {
        levels[i].x = pos_x;
        levels[i].y = pos_y;
        levels[i].w = 227;
        levels[i].h = 225;

        if((i+1)%2 == 0){
            pos_x = 10;
            pos_y += 229;
        }
        else{
            pos_x += 231;
        }
    }

    SDL_Event eventos;

    //Imagens do Menu
    SDL_Surface *default_char, *bacon, *hability;

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
            if(cur_select > 1)
                cur_select -= 2;
            teclas[UP] = false;
        }
        if(teclas[DOWN]){
            if(cur_select < 4)
                cur_select +=2;
            teclas[DOWN] = false;
        }

        if(teclas[LEFT]){
            if(cur_select > 0)
                cur_select--;
            teclas[LEFT] = false;
        }
        if(teclas[RIGHT]){
            if(cur_select < 5)
                cur_select ++;
            teclas[RIGHT] = false;
        }
        if(entra){
            return cur_select;
        }
        entra = false;

        select.x = levels[cur_select].x - 4;
        select.y = levels[cur_select].y - 4;

        SDL_BlitSurface(bacon, NULL, game.screenSurface, NULL);
        SDL_BlitSurface(lev_descript[cur_select], NULL, game.screenSurface, &hab);

        SDL_FillRect(game.screenSurface, &select, 0x5EFE18);

        for(i=0;i<6;i++)
        {
            SDL_BlitSurface(thumbs[i], NULL, game.screenSurface, &levels[i]);
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
