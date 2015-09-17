/**
  Autor: Fabio Lubacheski
  Objetivo: Desenha uma janela utilizando a biblioteca SDL 2.0
*/
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "game.h"

int mainloop(){

    int jogo_ativo = 1;
    int contagem = 0;
    bool pulo = false;
    typedef enum{UP, DOWN, LEFT, RIGHT};
    int impulso = 60;
    bool teclas[] = {false, false, false, false};

    SDL_Rect r = {10,10,32,32};
    SDL_Rect rcSprite = {0,0, 64,205};
    SDL_Event eventos;
    int velocidade_grav = 0;

    //Imagens do MainLoop
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

    while(jogo_ativo)
    {
        pulo = false;
        while(SDL_PollEvent(&eventos))
        {
            if(eventos.type == SDL_KEYDOWN)
            {
                if(eventos.key.keysym.sym == SDLK_DOWN){
                    printf("[ %5d ] DOWN \n", contagem++);
                    teclas[DOWN] = true;
                }
                if(eventos.key.keysym.sym == SDLK_SPACE){
                    printf("[ %5d ] DOWN \n", contagem++);
                    pulo = true;
                }
                if(eventos.key.keysym.sym == SDLK_UP){
                    printf("[ %5d ] UP \n", contagem++);
                    teclas[UP] = true;
                }
                if(eventos.key.keysym.sym == SDLK_LEFT){
                    printf("[ %5d ] LEFT\n", contagem++);
                    teclas[LEFT] = true;
                }
                if(eventos.key.keysym.sym == SDLK_RIGHT){
                    printf("[ %5d ] RIGHT  \n", contagem++);
                    teclas[RIGHT] = true;
                }
                else{
                    printf("Qualquer outra coisa \n",contagem++);
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
                else{
                    printf("Qualquer outra coisa \n",contagem++);
                }
            }
            if(eventos.type == SDL_QUIT){
                jogo_ativo = 0;
            }
        }

        if(teclas[UP]){
            r.y -= 1;
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

        if(pulo == true){
            r.y -= impulso;
            velocidade_grav = 0;
        }

        //GRAVIDADE LOUCA
        velocidade_grav += 1;
        r.y += velocidade_grav;

        //TRATAMENTO COLISAO BORDAS
        if(r.y < 0)
            r.y = 0;

        if(r.y + r.h > 600){
            r.y = 600 - r.h;
            velocidade_grav *= (-0.8);
        }

        if(r.x < 0)
            r.x = 0;

        if(r.x + r.w > 800)
            r.x = 800 - r.w;

        //printf("\n");
        SDL_FillRect(game.screenSurface, NULL, 0xffffff);
        SDL_BlitSurface(imagem, &rcSprite, game.screenSurface,&r);
        //SDL_FillRect(screenSurface, &r, 0xFF0000);
        SDL_UpdateWindowSurface(game.window);
        SDL_Delay(50);
    }
    return 0;
}
