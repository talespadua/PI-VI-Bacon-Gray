#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

typedef struct
{
    SDL_Window* window;
    SDL_Surface* screenSurface;
} Game;

#define ALTURA 1000
#define LARGURA 750

Game game;

#endif

