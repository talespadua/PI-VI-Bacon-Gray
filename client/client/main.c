#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <winsock.h>
#include <ctype.h>

#include "utils.h"
#include "game.h"

#define PORTA   5193
#define MAP_SIZE 25
#define SPRITE_SIZE 30

int initSDL();
void config();
void inicio(int socket_cliente);
void printMapa(int mapa[MAP_SIZE][MAP_SIZE]);
void receiveMap(int socket_cliente);

int num_jogador;
char address[16];

bool isPlayerOne = false;
int opponentAssit;

int main(int argc, char *argv[]) {
    int desc_socket;
    struct sockaddr_in endereco;

    initSDL();
    config();

    printf( "Iniciando WinSock API...\n" );
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(1, 1), &wsaData)!= 0) {
        printf("Cliente: WINSOCK.DLL nao encontrado \n" );
        exit( 1 );
    }

    if ((desc_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cliente: errro na criacao do socket \n");
        exit(1);
    }

    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(PORTA);
    endereco.sin_addr.s_addr = inet_addr(address);

    if (connect(desc_socket, (struct sockaddr *) &endereco, sizeof(endereco)) < 0) {
        printf("Cliente: erro na conexao \n");
        exit(1);
    }

    inicio(desc_socket);
    // jogo(desc_socket);

    system("pause");
    exit(0);
}

void inicio(int socket_cliente) {
    char buffer[1024];
    buffer[1023] = '\0';

    printf("Aguardando instruções do servidor...\n");
    recv(socket_cliente, buffer, 100, 0);
    printf("Recebido: |%s|\n", buffer);

    if (buffer[0] == ID_CHOOSE_PLAYER_1) {
        isPlayerOne = true;
    }

    /*
    printf("Choose your destiny:\n");
    scanf("%s", buffer);
    buffer[3] = '\0';
    buffer[2] = isPlayerOne ? buffer[1] : '0';
    buffer[1] = buffer[0];
    buffer[0] = ID_PLAYER_READY;
    //*/

    buffer[3] = '\0';
    buffer[2] = '1';
    buffer[1] = '1';
    buffer[0] = ID_PLAYER_READY;

    send(socket_cliente, buffer, strlen(buffer)+1, 0);

    while (true) {
        recv(socket_cliente, buffer, 100, 0);
        if (buffer[0] == ID_GAME_START) {
            // perform other operations (?)
            break;
        }
    }

    receiveMap(socket_cliente);
    // CreateThread(NULL, 0, (void*) receiveMap, socket_cliente, 0, NULL);
    // while(true);
    // receiveMap(socket_cliente);
    /*
    while (strcmp(buffer, "end")) {
        n = recv(socket_cliente, buffer, 627, 0);
        printf("Recebidos: %d\n", n);
    } //*/
}

void receiveMap(int socket_cliente) {
    int t1, t2, delay;
    int n, i = 0, j = 0, k = 1;
    delay = 100; //- 1000/25 ~= 40 FPS
    t1 = SDL_GetTicks();
    int mapa[MAP_SIZE][MAP_SIZE];
    char buffer[627];
    bool gameRunning = true;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Event eventos;
    SDL_Surface *obstacle, *player1, *player2, *monster, *floor, *win, *lose, *portrait;

    obstacle = SDL_LoadBMP("sprites/forest_wall.bmp");
    player1 = SDL_LoadBMP("sprites/player1.bmp");
    player2 = SDL_LoadBMP("sprites/player2.bmp");
    monster = SDL_LoadBMP("sprites/monstro.bmp");
    floor = SDL_LoadBMP("sprites/forest_floor.bmp");
    portrait = SDL_LoadBMP(isPlayerOne ? "sprites/player1_portrait.bmp" : "sprites/player2_portrait.bmp");
    win = SDL_LoadBMP(isPlayerOne ? "sprites/player1_win.bmp" : "sprites/player2_win.bmp");
    lose = SDL_LoadBMP("sprites/loser.bmp");

    SDL_Rect r = {0, 0, SPRITE_SIZE, SPRITE_SIZE};
    SDL_Rect rcSprite = {0, 0, SPRITE_SIZE, SPRITE_SIZE};
    SDL_Rect rcEndgame = {0, 0, 1000, 750};
    SDL_Rect rcPortrait = {760, 10, 230, 230};
    SDL_FillRect(game.screenSurface, NULL, 0x00ff00);

    SDL_BlitSurface(portrait, NULL, game.screenSurface, &rcPortrait);

    while (gameRunning) {
        char pressedKey;
        bool keyChanged = false;

        printf("Get user input\n");
        while (SDL_PollEvent(&eventos)) {
            if (eventos.type == SDL_KEYDOWN) {
                if (eventos.key.keysym.sym == SDLK_DOWN) {
                    pressedKey = DOWN;
                    keyChanged = true;
                }

                if (eventos.key.keysym.sym == SDLK_UP) {
                    pressedKey = UP;
                    keyChanged = true;
                }

                if (eventos.key.keysym.sym == SDLK_LEFT) {
                    pressedKey = LEFT;
                    keyChanged = true;
                }

                if (eventos.key.keysym.sym == SDLK_RIGHT) {
                    pressedKey = RIGHT;
                    keyChanged = true;
                }

                if (eventos.key.keysym.sym == SDLK_SPACE) {
                    // Especial
                }
            }

            if (eventos.type == SDL_QUIT){
                // Fim de jogo.
                // jogo_ativo = 0;
                exit(0);
            }
        }
        printf("End user input\n");

        // Envia movimentação do jogador
        if (keyChanged) {
            buffer[0] = ID_PLAYER_DIRECTION;
            buffer[1] = pressedKey;
            buffer[2] = '0';
            buffer[3] = '\0';

            printf("Send user input\n");
            send(socket_cliente, buffer, strlen(buffer)+1, 0);
        }

        printf("Request map\n");
        // Pede o mapa para o servidor
        buffer[0] = ID_REQUEST_MAP;
        buffer[1] = '\0';

        send(socket_cliente, buffer, strlen(buffer)+1, 0);
        printf("Map requested\n");
        n = recv(socket_cliente, buffer, 1024, 0);

        printf("Received: %d %s\n", n, buffer);

        if (buffer[0] == ID_MAP) {
            printf("Parse map\n");
            i = j = 0;

            for (k = 1; k < 626; k++) {
                mapa[i][j++] = buffer[k];
                if (j == 25) {
                    j = 0;
                    i++;
                }
            }

            printf("Blit map\n");
            for (j = 0; j < MAP_SIZE; j++) {
                for (i = 0; i < MAP_SIZE; i++) {
                    rcSprite.x = i * SPRITE_SIZE;
                    rcSprite.y = j * SPRITE_SIZE;

                    if (mapa[i][j] == OBSTACLE) {
                        SDL_BlitSurface(obstacle, &r, game.screenSurface, &rcSprite);
                    } else if (mapa[i][j] == PLAYER_ONE) {
                        SDL_BlitSurface(player1, &r, game.screenSurface, &rcSprite);
                    } else if (mapa[i][j] == PLAYER_TWO) {
                        SDL_BlitSurface(player2, &r, game.screenSurface, &rcSprite);
                    } else if (mapa[i][j] == MONSTER_BLUE) {
                        SDL_BlitSurface(monster, &r, game.screenSurface, &rcSprite);
                    } else {
                        SDL_BlitSurface(floor, &r, game.screenSurface, &rcSprite);
                    }
                }
            }

            printf("Update window\n");
            SDL_UpdateWindowSurface(game.window);
            // printf("return\n");
            // return;
        } else if (buffer[0] == ID_GAME_END) {
            if (buffer[1] == GAME_VICTORY) {
                SDL_BlitSurface(win, NULL, game.screenSurface, &rcEndgame);
                // win
            } else {
                SDL_BlitSurface(lose, NULL, game.screenSurface, &rcEndgame);
                // loose.
            }

            SDL_UpdateWindowSurface(game.window);
            gameRunning = false;
        }

        printf("Delay\n");
        // Calcula os milissegundos passados.
        t2 = SDL_GetTicks() - t1;

        // Espera o resto do tempo pra completar 1 frame
        if (t2 < delay) {
            SDL_Delay(delay - t2);
        }
        // SDL_UpdateWindowSurface(game.window);
        // SDL_Delay(1000);
    }
}

void config() {
    FILE *configFile;
    int i = 0;
    char data;
    char aux;

    configFile = fopen("config.txt", "r");
    if (!configFile) {
        printf("Erro ao abrir arquivo de configuracao!\n");
        exit(1);
    }

    while ((data = fgetc(configFile)) != EOF) {
        aux = (char) data;

        if (isspace(data)) {
            break;
        }

        address[i++] = aux;
    }

    address[i] = '\0';
    fclose(configFile);
}



int initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Nao foi possivel inicializar a SDL! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Create window
    game.window = SDL_CreateWindow("Sasha Bacon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ALTURA, LARGURA, SDL_WINDOW_SHOWN);
    if (game.window == NULL) {
        printf("Janela nao pode ser criada! SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        exit(2);
    }

    game.screenSurface = SDL_GetWindowSurface(game.window);
    return 1;
}

void printMapa(int mapa[MAP_SIZE][MAP_SIZE]) {
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
