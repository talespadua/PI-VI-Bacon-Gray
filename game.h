#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

typedef struct
{
    SDL_Window* window;
    SDL_Surface* screenSurface;
} Game;

#define ALTURA 800
#define LARGURA 600

Game game;

#endif

