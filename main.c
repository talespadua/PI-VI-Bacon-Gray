#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "mainloop.h"
#include "game.h"
#include "menu.h"

int init_sdl()
{    //- Inicializa a SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf("Nao foi possivel inicializar a SDL!  SDL_Error: %s\n", SDL_GetError() );
        return 1;
    }
    //Create window
    game.window = SDL_CreateWindow( "Sasha", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ALTURA, LARGURA, SDL_WINDOW_SHOWN );
    if( game.window == NULL )
    {
        printf("Janela nao pode ser criada!  SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return 2;
    }
    game.screenSurface = SDL_GetWindowSurface( game.window );
    return 0;
}

int main(int argc, char *argv[])
{
    init_sdl();
    menu();
    return 0;
}
