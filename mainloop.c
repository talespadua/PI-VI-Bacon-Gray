#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define TAMANHO_MAPA 11

void carrega_mapa(int mapa[][11], SDL_Window* window, SDL_Surface *screenSurface, SDL_Surface *parede,
                                     SDL_Surface *personagem, SDL_Surface *chao, int tamanho, int *x_jogador, int *y_jogador)
{
    SDL_Rect r = {0,0,tamanho,tamanho};
    SDL_Rect rcSprite = {0,0,tamanho,tamanho};

    SDL_FillRect(screenSurface, NULL, 0xffffff);

    int i, j;
    for(i = 0; i < TAMANHO_MAPA; i++)
    {
        for(j = 0; j < TAMANHO_MAPA; j++)
        {
            rcSprite.y = i*tamanho;
            rcSprite.x = j*tamanho;

            if(mapa[i][j] == -1)
                SDL_BlitSurface(parede, &r, screenSurface, &rcSprite);
            else if(mapa[i][j] == 1)
            {
                (*y_jogador) = i;
                (*x_jogador) = j;
                SDL_BlitSurface(personagem, &r, screenSurface, &rcSprite);
            }
            else
                SDL_BlitSurface(chao, &r, screenSurface, &rcSprite);

        }
    }

    SDL_UpdateWindowSurface(window);
}

int jogo(int argc, char *argv[]){
    int jogo_ativo = 1;
    typedef enum{UP, DOWN, LEFT, RIGHT};
    bool teclas[] = {false, false, false, false};

    int x_jogador, y_jogador;

    int tamanho = 30;
    bool movimento = false;

    SDL_Rect r = {0,0,tamanho,tamanho};
    SDL_Rect rcSprite = {0,0,tamanho,tamanho};

    int mapa[TAMANHO_MAPA][TAMANHO_MAPA] = {{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    SDL_Event eventos;

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL, *parede, *personagem, *chao;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Testando mapa", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

    screenSurface = SDL_GetWindowSurface(window);
    parede = SDL_LoadBMP("parede.bmp");
    personagem = SDL_LoadBMP("personagem.bmp");
    chao = SDL_LoadBMP("chao.bmp");

    carrega_mapa(mapa, window, screenSurface, parede, personagem, chao, tamanho, &x_jogador, &y_jogador);

    while(jogo_ativo)
    {
        while(SDL_PollEvent(&eventos))
        {
            if(eventos.type == SDL_KEYDOWN)
            {
                if(eventos.key.keysym.sym == SDLK_DOWN){
                    //printf("DOWN true\n");
                    teclas[DOWN] = true;
                }
                if(eventos.key.keysym.sym == SDLK_UP){
                    //printf("UP true\n");
                    teclas[UP] = true;
                }
                if(eventos.key.keysym.sym == SDLK_LEFT){
                    //printf("LEFT true\n");
                    teclas[LEFT] = true;
                }
                if(eventos.key.keysym.sym == SDLK_RIGHT){
                    //printf("RIGHT true\n");
                    teclas[RIGHT] = true;
                }
            }
            if(eventos.type == SDL_KEYUP)
            {
                if(eventos.key.keysym.sym == SDLK_DOWN){
                    //printf("DOWN false\n");
                    teclas[DOWN] = false;
                }
                if(eventos.key.keysym.sym == SDLK_UP){
                    //printf("UP false\n");
                    teclas[UP] = false;
                }
                if(eventos.key.keysym.sym == SDLK_LEFT){
                    //printf("LEFT false\n");
                    teclas[LEFT] = false;
                }
                if(eventos.key.keysym.sym == SDLK_RIGHT){
                    //printf("RIGHT false\n");
                    teclas[RIGHT] = false;
                }
            }
            if(eventos.type == SDL_QUIT){
                jogo_ativo = 0;
            }
        }

        r.x = x_jogador*tamanho;
        r.y = y_jogador*tamanho;
        if(teclas[DOWN])
        {
            if(mapa[y_jogador + 1][x_jogador] != -1 && y_jogador <= TAMANHO_MAPA - 2)
            {
                SDL_BlitSurface(chao, &rcSprite, screenSurface, &r);
                r.y = r.y + tamanho;
                y_jogador = y_jogador + 1;
                movimento = true;
            }
        }
        else if(teclas[UP])
        {
            if(mapa[y_jogador - 1][x_jogador] != -1 && y_jogador > 0)
            {
                SDL_BlitSurface(chao, &rcSprite, screenSurface, &r);
                r.y = r.y - tamanho;
                y_jogador = y_jogador - 1;
                movimento = true;
            }
        }
        else if(teclas[RIGHT])
        {
            if(mapa[y_jogador][x_jogador + 1] != -1 && x_jogador <= TAMANHO_MAPA - 2)
            {
                SDL_BlitSurface(chao, &rcSprite, screenSurface, &r);
                r.x = r.x + tamanho;
                x_jogador = x_jogador + 1;
                movimento = true;
            }
        }
        else if(teclas[LEFT])
        {
            if(mapa[y_jogador][x_jogador - 1] != -1 && x_jogador > 0)
            {
                SDL_BlitSurface(chao, &rcSprite, screenSurface, &r);
                r.x = r.x - tamanho;
                x_jogador = x_jogador - 1;
                movimento = true;
            }
        }



        if(movimento)
        {
            teclas[LEFT] = false;
            teclas[RIGHT] = false;
            teclas[UP] = false;
            teclas[DOWN] = false;
            movimento = false;

            /*
            //POR ALGUM MOTIVO BIZARRO, O MAPA ESTÁ INVERTIDO NO X E Y, MAS COMO ESTÁ AGORA FUNCIONA PERFEITAMENTE... :v
            printf("x/y: %i/%i\n", x_jogador, y_jogador);
            printf("acima: %i\n", mapa[y_jogador][x_jogador - 1]);
            printf("direita: %i\n", mapa[y_jogador + 1][x_jogador]);
            printf("abaixo: %i\n", mapa[y_jogador][x_jogador + 1]);
            printf("esquerda: %i\n", mapa[y_jogador - 1][x_jogador]);
            */

            SDL_BlitSurface(personagem, &rcSprite, screenSurface, &r);
        }
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
