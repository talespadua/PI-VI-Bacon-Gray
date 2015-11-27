#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

#include "game.h"
#include "queue.h"
#include "player.h"
#include "monster.h"

#define MAP_SIZE 20
#define PLAYER_WEIGHT -2

void printMapa(int mapa[][MAP_SIZE]);
void copyMatrix(int a[][MAP_SIZE], int b[][MAP_SIZE]);
void enqueueNeighbors(Queue *q, Node *n, int mapa[][MAP_SIZE]);
void smallestNeighbor(int mapa[][MAP_SIZE], int *x, int *y);
void enqueueBestWay(Queue *q, Node *s, Node *t, int mapa[][MAP_SIZE]);
void calculate(Queue *a, Queue *b, int mapa[][MAP_SIZE]);

void carrega_mapa(int mapa[][MAP_SIZE], SDL_Window* window, SDL_Surface *screenSurface,
                  SDL_Surface *parede, SDL_Surface *personagem, SDL_Surface *chao,
                  int tamanho, int *x_jogador, int *y_jogador) {

    FILE *mapFile;
    int square;
    int i, j;

    mapFile = fopen("mapa.txt", "r");
    SDL_Rect r = {0,0,tamanho,tamanho};
    SDL_Rect rcSprite = {0,0,tamanho,tamanho};
    SDL_FillRect(screenSurface, NULL, 0xffffff);

    for(j = 0; j < MAP_SIZE; j++) {
        for(i = 0; i < MAP_SIZE; i++) {
            fscanf(mapFile, "%d", &square);
            printf("%d ", square);
            mapa[i][j] = square;
        }
        printf("\n");
    }

    for (j = 0; j < MAP_SIZE; j++) {
        for (i = 0; i < MAP_SIZE; i++) {
            rcSprite.x = i*tamanho;
            rcSprite.y = j*tamanho;

            if (mapa[i][j] == -1) {
                SDL_BlitSurface(parede, &r, screenSurface, &rcSprite);
            } else if(mapa[i][j] == PLAYER_WEIGHT){
                (*x_jogador) = i;
                (*y_jogador) = j;
                SDL_BlitSurface(personagem, &r, screenSurface, &rcSprite);
            } else {
                SDL_BlitSurface(chao, &r, screenSurface, &rcSprite);
            }
        }
    }

    SDL_UpdateWindowSurface(window);
}

int jogo(int argc, char *argv[]){
    int jogo_ativo = 1;

    int t1, t2, t3, delay, timeCounter, framesPassed;
    timeCounter = 0;
    delay = 16; //- 1000/16 ~= 60 FPS
    t1 = SDL_GetTicks();
    framesPassed = 0;
    bool playerWalk = true;
    bool monsterWalk = false;

    int monsterBaseSpeed = MONSTER_BASE_SPEED;
    MonsterList *ml = monster_create();
    // monster_add(ml, 14, 19, 0, monsterBaseSpeed, 0);

    PlayerList *pl = player_create();
    player_add(pl, 1, 0, 0, 0, 0, "lol");
    Player *p = pl->first;
    bool special = false;

    typedef enum{UP, DOWN, LEFT, RIGHT};
    bool teclas[] = {false, false, false, false};
    int morto = false;

    // int x_jogador, y_jogador;
    // int x_monstro = 14, y_monstro = 19;

    int tamanho = 30;
    bool movimento = false;

    SDL_Rect r = {0,0,tamanho,tamanho};
    SDL_Rect rcSprite = {0,0,tamanho,tamanho};

    int mapa[MAP_SIZE][MAP_SIZE];

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

    carrega_mapa(mapa, game.window, game.screenSurface, parede, personagem, chao, tamanho, &p->x, &p->y);

    while (jogo_ativo) {
        while (SDL_PollEvent(&eventos)) {
            if (eventos.type == SDL_KEYDOWN) {
                if (eventos.key.keysym.sym == SDLK_DOWN) {
                    //printf("DOWN true\n");
                    // Teste
                    teclas[LEFT] = false;
                    teclas[RIGHT] = false;
                    teclas[UP] = false;

                    teclas[DOWN] = true;
                }

                if (eventos.key.keysym.sym == SDLK_UP) {
                    //printf("UP true\n");
                    // Teste
                    teclas[LEFT] = false;
                    teclas[RIGHT] = false;
                    teclas[DOWN] = false;

                    teclas[UP] = true;
                }

                if (eventos.key.keysym.sym == SDLK_LEFT) {
                    //printf("LEFT true\n");
                    // Teste
                    teclas[RIGHT] = false;
                    teclas[UP] = false;
                    teclas[DOWN] = false;

                    teclas[LEFT] = true;
                }

                if (eventos.key.keysym.sym == SDLK_RIGHT) {
                    //printf("RIGHT true\n");
                    // Teste
                    teclas[LEFT] = false;
                    teclas[UP] = false;
                    teclas[DOWN] = false;

                    teclas[RIGHT] = true;
                }

                if (eventos.key.keysym.sym == SDLK_SPACE) {
                    //printf("RIGHT true\n");
                    special = true;
                }
            }

            /*
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
            } //*/

            if (eventos.type == SDL_QUIT){
                jogo_ativo = 0;
            }
        }

        playerWalk = (framesPassed % p->speed) == 0;
        r.x = p->x * tamanho;
        r.y = p->y * tamanho;

        if (!morto && playerWalk) {
            if (teclas[DOWN]) {
                if(mapa[p->x][p->y+1] != -1 && p->y <= MAP_SIZE - 2) {
                    SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                    // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                    mapa[p->x][p->y] = 0;

                    r.y = r.y + tamanho;
                    p->y++;
                    movimento = true;
                }
            } else if(teclas[UP]) {
                if(mapa[p->x][p->y-1] != -1 && p->y > 0) {
                    SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                    // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                    mapa[p->x][p->y] = 0;

                    r.y = r.y - tamanho;
                    p->y--;
                    movimento = true;
                }
            } else if(teclas[RIGHT]) {
                if(mapa[p->x+1][p->y] != -1 && p->x <= MAP_SIZE - 2) {
                    SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                    // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                    mapa[p->x][p->y] = 0;

                    r.x = r.x + tamanho;
                    p->x++;
                    movimento = true;
                }
            } else if(teclas[LEFT]) {
                if(mapa[p->x-1][p->y] != -1 && p->x > 0) {
                    SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                    // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                    mapa[p->x][p->y] = 0;

                    r.x = r.x - tamanho;
                    p->x--;
                    movimento = true;
                }
            }

            // if (movimento) {
            if (movimento) {
                //teclas[LEFT] = false;
                //teclas[RIGHT] = false;
                //teclas[UP] = false;
                //teclas[DOWN] = false;
                //movimento = false;

                /*
                //POR ALGUM MOTIVO BIZARRO, O MAPA ESTÁ INVERTIDO NO X E Y, MAS COMO ESTÁ AGORA FUNCIONA PERFEITAMENTE... :v
                printf("x/y: %i/%i\n", x_jogador, y_jogador);
                printf("acima: %i\n", mapa[y_jogador][x_jogador - 1]);
                printf("direita: %i\n", mapa[y_jogador + 1][x_jogador]);
                printf("abaixo: %i\n", mapa[y_jogador][x_jogador + 1]);
                printf("esquerda: %i\n", mapa[y_jogador - 1][x_jogador]);
                */
                //SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);
                SDL_BlitSurface(personagem, &rcSprite, game.screenSurface, &r);

                // Limpa a posição no mapa para facilitar o cálculo do pathfinding
                mapa[p->x][p->y] = -2;
            }

            if (special && !monster_isEmpty(ml)) {
                special = false;

                Monster *m = ml->first;
                monster_remove(ml, m);

                mapa[m->x][m->y] = 0;
                r.x = m->x*tamanho;
                r.y = m->y*tamanho;
                SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

                free(m);
            }
        }

        Monster *m = ml->first;
        while (m) {
            monsterWalk = (framesPassed % m->speed) == 0;

            // movimento do monstro
            if (!monsterWalk) {
                m = m->next;
                continue;
            }

            mapa[m->x][m->y] = 0;

            queue_clear(astar);
            queue_clear(way);
            queue_enq(astar, m->x, m->y, 0);
            queue_enq(astar, p->x, p->y, 0);

            // printf("astar:\n");
            // queue_print(astar);
            calculate(astar, way, mapa);

            // printf("Way:\n");
            // queue_print(way);

            r.x = m->x*tamanho;
            r.y = m->y*tamanho;
            SDL_BlitSurface(chao, &rcSprite, game.screenSurface, &r);

            if (!queue_isEmpty(way)) {
                Node *n = queue_deq(way);
                m->x = n->x;
                m->y = n->y;

                // printf("Node: ");
                // queue_printNode(n);
            }

            r.x = m->x*tamanho;
            r.y = m->y*tamanho;

            SDL_BlitSurface(monstro, &rcSprite, game.screenSurface, &r);
            // printf("\n");

            mapa[m->x][m->y] = -1;

            if (m->x == p->x && m->y == p->y) {
                morto = true;
            }

            m = m->next;
        }

        if (morto) {
            break;
        }

        //- calcula os milissegundos passados
        t2 = SDL_GetTicks() - t1;
        SDL_UpdateWindowSurface(game.window);

        //*
        if (t2 < delay){
            //- espera o resto do tempo
            SDL_Delay(delay - t2);
        } //*/

        framesPassed = (framesPassed++) % (1000);

        //*
        if (t2 > 1000) {
            t1 = SDL_GetTicks();

            if ((timeCounter % 5) == 0) {
                monsterBaseSpeed -= 20;
                monster_add(ml, 9, 9, 0, monsterBaseSpeed, 0);
            }

            timeCounter++;
        }
        //*/
    }

    queue_free(astar);
    queue_free(way);
    monster_free(ml);
    player_free(pl);

    //SDL_DestroyWindow( game.window );
    //SDL_Quit();

    return 0;
}

void printMapa(int mapa[][MAP_SIZE]) {
    int i, j;
    for (i = 0; i < MAP_SIZE; i++) {
        for (j = 0; j < MAP_SIZE; j++) {
            if (mapa[i][j] < 0 || mapa[i][j] > 9)
                printf("%d, ", mapa[i][j]);
            else
                printf(" %d, ", mapa[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}

void copyMatrix(int a[][MAP_SIZE], int b[][MAP_SIZE]) {
    int i, j;

    for (i = 0; i < MAP_SIZE; i++)
        for (j = 0; j < MAP_SIZE; j++)
            b[i][j] = a[i][j];
}

void enqueueNeighbors(Queue *q, Node *n, int mapa[][MAP_SIZE]) {
    int x = n->x, y = n->y, w = n->w +1;
    int xn, yn, aux;
    int border = MAP_SIZE-1;

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

void smallestNeighbor(int mapa[][MAP_SIZE], int *rx, int *ry) {
    int a, b, xn, yn, aux, smallest = INT_MAX;
    int x = (*rx), y = (*ry);
    int border = MAP_SIZE-1;

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

    // printf("smallest: %d\n", smallest);
    (*rx) = a;
    (*ry) = b;
}

void enqueueBestWay(Queue *q, Node *s, Node *t, int mapa[][MAP_SIZE]) {
    int xs = s->x, ys = s->y;
    int xt = t->x, yt = t->y;
    int x = xs, y = ys;

    // printf ("inicial: x:%d, y:%d\n", x, y);
    // printf ("destino: x:%d, y:%d\n", xt, yt);

    // while ((x != xt) || (y != yt)) {
    // while (1) {
        smallestNeighbor(mapa, &x, &y);
        queue_enq(q, x, y, 0);
        // printf ("x:%d, y:%d\n", x, y);

        // if (x == xt && y == yt) break;
    // }
}

void calculate(Queue *a, Queue *b, int mapa[][MAP_SIZE]) {
    Node *monster = queue_deq(a);
    Node *player = queue_peek(a);

    int mapaAux[MAP_SIZE][MAP_SIZE];
    copyMatrix(mapa, mapaAux);

    // printMapa(mapaAux);

    while (!queue_isEmpty(a))
        enqueueNeighbors(a, queue_deq(a), mapaAux);

    // printMapa(mapaAux);

    // return;
    // printf("player: ");
    // queue_printNode(player);
    // printf("monster: ");
    // queue_printNode(monster);
    enqueueBestWay(b, monster, player, mapaAux);
    // queue_print(b);
}
