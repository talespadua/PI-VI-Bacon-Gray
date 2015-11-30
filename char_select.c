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

    int pos_x = 10, pos_y = 10, hab_num = 0;
    int cur_char = 0;

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

    SDL_Surface *char_img[9];
    char_img[0] = SDL_LoadBMP("papaleguas.bmp");
    char_img[1] = SDL_LoadBMP("reagan.bmp");
    char_img[2] = SDL_LoadBMP("vini.bmp");
    char_img[3] = SDL_LoadBMP("scorpion.bmp");
    char_img[4] = SDL_LoadBMP("tati.bmp");
    char_img[5] = SDL_LoadBMP("wander.bmp");
    char_img[6] = SDL_LoadBMP("zeca.bmp");
    char_img[7] = SDL_LoadBMP("frozen.bmp");
    char_img[8] = SDL_LoadBMP("tales.bmp");

    SDL_Surface *habilidades[9];
    habilidades[0] = SDL_LoadBMP("papaleguas_hab.bmp");
    habilidades[1] = SDL_LoadBMP("reagan_hab.bmp");
    habilidades[2] = SDL_LoadBMP("vini_hab.bmp");
    habilidades[3] = SDL_LoadBMP("scorpion_hab.bmp");
    habilidades[4] = SDL_LoadBMP("tati_hab.bmp");
    habilidades[5] = SDL_LoadBMP("wander_hab.bmp");
    habilidades[6] = SDL_LoadBMP("zeca_hab.bmp");
    habilidades[7] = SDL_LoadBMP("elsa_hab.bmp");
    habilidades[8] = SDL_LoadBMP("tales_hab.bmp");

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
            if(cur_char > 2)
                cur_char -= 3;
            teclas[UP] = false;
        }
        if(teclas[DOWN]){
            if(cur_char < 6)
                cur_char += 3;
            teclas[DOWN] = false;
        }

        if(teclas[LEFT]){
            if(cur_char >0)
                cur_char--;
            teclas[LEFT] = false;
        }
        if(teclas[RIGHT]){
            if(cur_char < 8)
                cur_char ++;
            teclas[RIGHT] = false;
        }
        if(entra){
            return cur_char;
        }
        entra = false;

        select.x = chars[cur_char].x - 4;
        select.y = chars[cur_char].y - 4;

        SDL_BlitSurface(bacon, NULL, game.screenSurface, NULL);
        SDL_BlitSurface(habilidades[cur_char], NULL, game.screenSurface, &hab);

        SDL_FillRect(game.screenSurface, &select, 0x5EFE18);

        for(i=0;i<9;i++)
        {
            SDL_BlitSurface(char_img[i], NULL, game.screenSurface, &chars[i]);
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
