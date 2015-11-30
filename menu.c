#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "mainloop.h"
#include "char_select.h"

void menu()
{
    bool menu_ativo = true, entra = false;
    typedef enum{UP, DOWN, LEFT, RIGHT};
    bool teclas[] = {false, false, false, false};

    int personagem, level;

    SDL_Rect ini_ret = {350,300,163,56};
    SDL_Rect sair_ret = {350,380, 107,56};
    //SDL_Rect name_ret = {0,0, 800,200};
    SDL_Rect select = {348,298,167,60};

    SDL_Event eventos;

    //Imagens do Menu
    SDL_Surface *iniciar, *sair, *name, *sasha_bacon;

    iniciar = SDL_LoadBMP("iniciar.bmp");
    sair = SDL_LoadBMP("sair.bmp");
    //name = SDL_LoadBMP("name.bmp");
    sasha_bacon = SDL_LoadBMP("sasha_bacon.bmp");

    //SDL_SetColorKey(sasha_bacon, SDL_TRUE, SDL_MapRGB(sasha_bacon->format, 255, 255, 255));
    //SDL_SetColorKey(imagem, SDL_TRUE, SDL_MapRGB(imagem->format, 0, 255, 255));
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
                if(eventos.key.keysym.sym == SDLK_RETURN){
                      entra = false;
                }
            }
            if(eventos.type == SDL_QUIT){
                menu_ativo = 0;
            }
        }

        if(teclas[UP]){
            //mainloop();
            select.y = ini_ret.y - 2;
            select.w = ini_ret.w + 4;
        }
        if(teclas[DOWN]){
            select.y = sair_ret.y-2;
            select.w = sair_ret.w + 4;
        }
        if(entra){
            if(select.y == ini_ret.y - 2){
                //jogo();
                personagem = char_select();
                //printf("personagem %d\n", personagem);
                level = level_select();
                //printf("level %d\n", level);
                jogo(personagem, level);
            }
            if(select.y == sair_ret.y - 2){
                exit(0);
            }
        }
        entra = false;

        SDL_BlitSurface(sasha_bacon, NULL, game.screenSurface, NULL);
        SDL_FillRect(game.screenSurface, &select, 0xFF0000);
        SDL_BlitSurface(iniciar, NULL, game.screenSurface, &ini_ret);
        SDL_BlitSurface(sair, NULL, game.screenSurface, &sair_ret);
        //SDL_BlitSurface(name, NULL, game.screenSurface, &name_ret);



        /*SDL_RenderCopy(screenRenderer, sasha_textura, NULL, NULL);
        SDL_RenderCopy(screenRenderer, iniciar_textura, NULL, &ini_ret);
        SDL_RenderCopy(screenRenderer, sair_textura, NULL, &sair_ret);
        SDL_RenderCopy(screenRenderer, name_textura, NULL, &name_ret);*/
        SDL_UpdateWindowSurface(game.window);
        SDL_Delay(50);
    }
    //Destroi window
    SDL_DestroyWindow( game.window );

    // desliga todos os subsistemas da SDL e libera recursos alocados a eles.
    // Esta função sempre deve ser chamada antes de você sair.
    SDL_Quit();
    return 0;
}
