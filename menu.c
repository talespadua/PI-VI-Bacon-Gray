#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"

void menu()
{
    bool menu_ativo = true;
    typedef enum{UP, DOWN, LEFT, RIGHT};
    bool teclas[] = {false, false, false, false};

    //TTF_Font *gFont = NULL;

    SDL_Rect r = {10,10,32,32};
    SDL_Rect rcSprite = {0,0, 64,205};
    SDL_Event eventos;

    //Imagens do Menu
    SDL_Surface *imagem, *sprite;

    imagem = SDL_LoadBMP("desenho.bmp");

    if( imagem == NULL )
    {
        printf("Imagem nao pode ser carregada!  SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return 2;
    }
    SDL_SetColorKey(imagem, SDL_TRUE, SDL_MapRGB(imagem->format, 0, 255, 255));
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
            }
            if(eventos.type == SDL_KEYUP)
            {
                if(eventos.key.keysym.sym == SDLK_DOWN){
                    teclas[DOWN] = false;
                }
                if(eventos.key.keysym.sym == SDLK_UP){
                    teclas[UP] = false;
                }
            }
            if(eventos.type == SDL_QUIT){
                menu_ativo = 0;
            }
        }

        if(teclas[UP]){
            mainloop();
        }
        if(teclas[DOWN]){
            r.y+=1;
        }
        if(teclas[LEFT]){
            r.x-=1;
            rcSprite.x -=64;
            if(rcSprite.x <= 0)
                rcSprite.x = 192;
        }
        if(teclas[RIGHT]){
            r.x+=1;
            rcSprite.x+=64;
            if(rcSprite.x >= 256)
                rcSprite.x = 0;
        }

        //printf("\n");
        SDL_FillRect(game.screenSurface, NULL, 0xffffff);
        //SDL_FillRect(screenSurface, &r, 0xFF0000);
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
