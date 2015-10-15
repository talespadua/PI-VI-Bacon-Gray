#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

#include "game.h"
#include "queue.h"

#define TAMANHO_MAPA 20
#define PLAYER_WEIGHT -2

void printMapa(int mapa[][TAMANHO_MAPA]);
void copyMatrix(int a[][TAMANHO_MAPA], int b[][TAMANHO_MAPA]);
void enqueueNeighbors(Queue *q, Node *n, int mapa[][TAMANHO_MAPA]);
void smallestNeighbor(int mapa[][TAMANHO_MAPA], int *x, int *y);
void enqueueBestWay(Queue *q, Node *s, Node *t, int mapa[][TAMANHO_MAPA]);
void calculate(Queue *a, Queue *b, int mapa[][TAMANHO_MAPA]);

void carrega_mapa(int mapa[][TAMANHO_MAPA], SDL_Window* window, SDL_Surface *screenSurface, SDL_Surface *parede,
                                     SDL_Surface *personagem, SDL_Surface *chao, int tamanho, int *x_jogador, int *y_jogador) {
    SDL_Rect r = {0,0,tamanho,tamanho};
    SDL_Rect rcSprite = {0,0,tamanho,tamanho};

    SDL_FillRect(screenSurface, NULL, 0xffffff);

    int i, j;
    for(i = 0; i < TAMANHO_MAPA; i++) {
        for(j = 0; j < TAMANHO_MAPA; j++) {
            rcSprite.y = i*tamanho;
            rcSprite.x = j*tamanho;

            if(mapa[i][j] == -1)
                SDL_BlitSurface(parede, &r, screenSurface, &rcSprite);
            else if(mapa[i][j] == PLAYER_WEIGHT) {
                (*y_jogador) = i;
                (*x_jogador) = j;
                SDL_BlitSurface(personagem, &r, screenSurface, &rcSprite);
            } else
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
    int x_monstro = 14, y_monstro = 19;

    int tamanho = 30;
    bool movimento = false;

    SDL_Rect r = {0,0,tamanho,tamanho};
    SDL_Rect rcSprite = {0,0,tamanho,tamanho};

    int mapa[TAMANHO_MAPA][TAMANHO_MAPA] = {
        {0, 0, 0, 0, 0, PLAYER_WEIGHT, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
        {0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0},
        {0, -1, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, -1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0},
        {0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, -1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        };

    SDL_Event eventos;

    //SDL_Window* window = NULL;
    SDL_Surface *parede, *personagem, *monstro, *chao;

    Queue *astar = queue_create();
    Queue *way = queue_create();

    SDL_Init(SDL_INIT_VIDEO);
    //window = SDL_CreateWindow("Testando mapa", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

    //screenSurface = SDL_GetWindowSurface(game.window);
    parede = SDL_LoadBMP("parede.bmp");
    personagem = SDL_LoadBMP("personagem.bmp");
    monstro = SDL_LoadBMP("monstro.bmp");
    chao = SDL_LoadBMP("chao.bmp");

    carrega_mapa(mapa, game.window, game.screenSurface, parede, personagem, chao, tamanho, &x_jogador, &y_jogador);

    while (jogo_ativo) {
        while (SDL_PollEvent(&eventos)) {
            if (eventos.type == SDL_KEYDOWN) {
                if (eventos.key.keysym.sym == SDLK_DOWN) {
                    //printf("DOWN true\n");
                    teclas[DOWN] = true;
                }

                if (eventos.key.keysym.sym == SDLK_UP) {
                    //printf("UP true\n");
                    teclas[UP] = true;
                }

                if (eventos.key.keysym.sym == SDLK_LEFT) {
                    //printf("LEFT true\n");
                    teclas[LEFT] = true;
                }

                if (eventos.key.keysym.sym == SDLK_RIGHT) {
                    //printf("RIGHT true\n");
                    teclas[RIGHT] = true;
                }
            }

            if (eventos.type == SDL_KEYUP) {
                if(eventos.key.keysym.sym == SDLK_DOWN) {
                    //printf("DOWN false\n");
                    teclas[DOWN] = false;
                }

                if (eventos.key.keysym.sym == SDLK_UP) {
                    //printf("UP false\n");
                    teclas[UP] = false;
                }

                if (eventos.key.keysym.sym == SDLK_LEFT) {
                    //printf("LEFT false\n");
                    teclas[LEFT] = false;
                }

                if (eventos.key.keysym.sym == SDLK_RIGHT) {
                    //printf("RIGHT false\n");
                    teclas[RIGHT] = false;
                }
            }

            if (eventos.type == SDL_QUIT){
                jogo_ativo = 0;
            }
        }

        r.x = x_jogador*tamanho;
        r.y = y_jogador*tamanho;
        if (teclas[DOWN]) {
            if(mapa[y_jogador + 1][x_jogador] != -1 && y_jogador <= TAMANHO_MAPA - 2) {
                SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                mapa[y_jogador][x_jogador] = 0;

                r.y = r.y + tamanho;
                y_jogador = y_jogador + 1;
                movimento = true;
            }
        } else if(teclas[UP]) {
            if(mapa[y_jogador - 1][x_jogador] != -1 && y_jogador > 0) {
                SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                mapa[y_jogador][x_jogador] = 0;

                r.y = r.y - tamanho;
                y_jogador = y_jogador - 1;
                movimento = true;
            }
        } else if(teclas[RIGHT]) {
            if(mapa[y_jogador][x_jogador + 1] != -1 && x_jogador <= TAMANHO_MAPA - 2) {
                SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                mapa[y_jogador][x_jogador] = 0;

                r.x = r.x + tamanho;
                x_jogador = x_jogador + 1;
                movimento = true;
            }
        } else if(teclas[LEFT]) {
            if(mapa[y_jogador][x_jogador - 1] != -1 && x_jogador > 0) {
                SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                mapa[y_jogador][x_jogador] = 0;

                r.x = r.x - tamanho;
                x_jogador = x_jogador - 1;
                movimento = true;
            }
        }



        if (movimento) {
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

            SDL_BlitSurface(personagem, &rcSprite, game.screenSurface, &r);

            // Limpa a posição no mapa para facilitar o cálculo do pathfinding
            mapa[y_jogador][x_jogador] = -2;

            // movimento do monstro
            // x, y, w
            queue_clear(astar);
            queue_clear(way);
            queue_enq(astar, x_monstro, y_monstro, 0);
            queue_enq(astar, y_jogador, x_jogador, 0);

            printf("astar:\n");
            queue_print(astar);
            calculate(astar, way, mapa);

            printf("Way:\n");
            queue_print(way);

            r.x = y_monstro*tamanho;
            r.y = x_monstro*tamanho;
            SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

            if (!queue_isEmpty(way)) {
                Node *n = queue_deq(way);
                x_monstro = n->x;
                y_monstro = n->y;

                printf("Node: ");
                queue_printNode(n);
            }

            r.x = y_monstro*tamanho;
            r.y = x_monstro*tamanho;

            SDL_BlitSurface(monstro, &rcSprite, game.screenSurface, &r);
            printf("\n");
        }

        SDL_UpdateWindowSurface(game.window);
    }

    queue_free(astar);
    queue_free(way);

    //SDL_DestroyWindow( game.window );
    //SDL_Quit();

    return 0;
}

void printMapa(int mapa[][TAMANHO_MAPA]) {
    int i, j;
    for (i = 0; i < TAMANHO_MAPA; i++) {
        for (j = 0; j < TAMANHO_MAPA; j++) {
            if (mapa[i][j] < 0 || mapa[i][j] > 9)
                printf("%d, ", mapa[i][j]);
            else
                printf(" %d, ", mapa[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}

void copyMatrix(int a[][TAMANHO_MAPA], int b[][TAMANHO_MAPA]) {
    int i, j;

    for (i = 0; i < TAMANHO_MAPA; i++)
        for (j = 0; j < TAMANHO_MAPA; j++)
            b[i][j] = a[i][j];
}

void enqueueNeighbors(Queue *q, Node *n, int mapa[][TAMANHO_MAPA]) {
    int x = n->x, y = n->y, w = n->w +1;
    int xn, yn, aux;
    int border = TAMANHO_MAPA-1;

    // (x-1, y) == cima
    if (x > 0) {
        xn = x-1;
        yn = y;
        aux = mapa[xn][yn];

        if (aux == 0 || aux > w) {
            queue_enq(q, xn, yn, w);
            mapa[xn][yn] = w;
        }
    }

    // (x+1, y) baixo
    if (x < border) {
        xn = x+1;
        yn = y;
        aux = mapa[xn][yn];

        if (aux == 0 || aux > w) {
            queue_enq(q, xn, yn, w);
            mapa[xn][yn] = w;
        }
    }

    // (x, y-1) esquerda
    if (y > 0) {
        xn = x;
        yn = y-1;
        aux = mapa[xn][yn];

        if (aux == 0 || aux > w) {
            queue_enq(q, xn, yn, w);
            mapa[xn][yn] = w;
        }
    }

    // (x, y+1) direita
    if (y < border) {
        xn = x;
        yn = y+1;
        aux = mapa[xn][yn];

        if (aux == 0 || aux > w) {
            queue_enq(q, xn, yn, w);
            mapa[xn][yn] = w;
        }
    }
}

void smallestNeighbor(int mapa[][TAMANHO_MAPA], int *rx, int *ry) {
    int a, b, xn, yn, aux, smallest = INT_MAX;
    int x = (*rx), y = (*ry);
    int border = TAMANHO_MAPA-1;

    // (x-1, y) == cima
    if (x > 0) {
        xn = x-1;
        yn = y;

        aux = mapa[xn][yn];
        if (aux != -1 && aux < smallest) {
            smallest = aux;
            a = xn;
            b = yn;
        }
    }

    // (x+1, y) baixo
    if (x < border) {
        xn = x+1;
        yn = y;

        aux = mapa[xn][yn];
        if (aux != -1 && aux < smallest) {
            smallest = aux;
            a = xn;
            b = yn;
        }
    }

    // (x, y-1) esquerda
    if (y > 0) {
        xn = x;
        yn = y-1;

        aux = mapa[xn][yn];
        if (aux != -1 && aux < smallest) {
            smallest = aux;
            a = xn;
            b = yn;
        }
    }

    // (x, y+1) direita
    if (y < border) {
        xn = x;
        yn = y+1;

        aux = mapa[xn][yn];
        if (aux != -1 && aux < smallest) {
            smallest = aux;
            a = xn;
            b = yn;
        }
    }

    printf("smallest: %d\n", smallest);
    (*rx) = a;
    (*ry) = b;
}

void enqueueBestWay(Queue *q, Node *s, Node *t, int mapa[][TAMANHO_MAPA]) {
    int xs = s->x, ys = s->y;
    int xt = t->x, yt = t->y;
    int x = xs, y = ys;

    printf ("inicial: x:%d, y:%d\n", x, y);
    printf ("destino: x:%d, y:%d\n", xt, yt);

    while ((x != xt) || (y != yt)) {
    // while (1) {
        smallestNeighbor(mapa, &x, &y);
        queue_enq(q, x, y, 0);
        // printf ("x:%d, y:%d\n", x, y);

        // if (x == xt && y == yt) break;
    }
}

void calculate(Queue *a, Queue *b, int mapa[][TAMANHO_MAPA]) {
    Node *monster = queue_deq(a);
    Node *player = queue_peek(a);

    int mapaAux[TAMANHO_MAPA][TAMANHO_MAPA];
    copyMatrix(mapa, mapaAux);

    // printMapa(mapaAux);

    while (!queue_isEmpty(a))
        enqueueNeighbors(a, queue_deq(a), mapaAux);

    // printMapa(mapaAux);

    // return;
    printf("player: ");
    queue_printNode(player);
    printf("monster: ");
    queue_printNode(monster);
    enqueueBestWay(b, monster, player, mapaAux);
    // queue_print(b);
}
