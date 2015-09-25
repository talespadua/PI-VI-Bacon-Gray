#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"

void menu()
{
    bool menu_ativo = true, entra = false;
    typedef enum{UP, DOWN, LEFT, RIGHT};
    bool teclas[] = {false, false, false, false};

    //TTF_Font *gFont = NULL;

    SDL_Renderer *screenRenderer = NULL;

    SDL_Rect ini_ret = {350,300,163,56};
    SDL_Rect sair_ret = {350,380, 107,56};
    SDL_Rect name_ret = {0,0, 800,200};
    SDL_Rect select = {350,300,163,56};

    SDL_Event eventos;
    screenRenderer = SDL_CreateRenderer( game.window, -1, SDL_RENDERER_ACCELERATED );
    //Imagens do Menu
    SDL_Surface *iniciar, *sair, *name;
    SDL_Texture *iniciar_textura, *sair_textura, *name_textura;


    iniciar = SDL_LoadBMP("iniciar.bmp");
    iniciar_textura = SDL_CreateTextureFromSurface( screenRenderer, iniciar );
    SDL_FreeSurface(iniciar);

    sair = SDL_LoadBMP("sair.bmp");
    sair_textura = SDL_CreateTextureFromSurface( screenRenderer, sair );
    SDL_FreeSurface(sair);

    name = SDL_LoadBMP("name.bmp");
    name_textura = SDL_CreateTextureFromSurface( screenRenderer, name );
    SDL_FreeSurface(name);



    if( name == NULL )
    {
        printf("Imagem nao pode ser carregada!  SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return 2;
    }

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
            select.y = ini_ret.y;
        }
        if(teclas[DOWN]){
            select.y = sair_ret.y;
        }
        if(entra){
            if(select.y == ini_ret.y){
                mainloop();
            }
            if(select.y == sair_ret.y){
                exit(0);
            }
        }
        SDL_SetRenderDrawColor( screenRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear(screenRenderer);

        //SDL_FillRect(screenSurface, &r, 0xFF0000);
        SDL_RenderCopy(screenRenderer, iniciar_textura, NULL, &ini_ret/*&rectDestino*/);
        SDL_RenderCopy(screenRenderer, sair_textura, NULL, &sair_ret/*&rectDestino*/);
        SDL_RenderCopy(screenRenderer, name_textura, NULL, &name_ret/*&rectDestino*/);

        SDL_SetRenderDrawColor( screenRenderer, 0xFF, 0x00, 0x00, 0xFF );
        SDL_RenderDrawRect( screenRenderer, &select );

        SDL_RenderPresent(screenRenderer);
        SDL_Delay(50);
    }
    //Destroi window
    SDL_DestroyWindow( game.window );

    // desliga todos os subsistemas da SDL e libera recursos alocados a eles.
    // Esta função sempre deve ser chamada antes de você sair.
    SDL_Quit();
    return 0;
}
