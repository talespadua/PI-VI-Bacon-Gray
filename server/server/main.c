#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <winsock.h>
#include <limits.h>

#include "utils.h"
#include "queue.h"
#include "player.h"
#include "monster.h"

#define ADDRESS  "0.0.0.0"
#define PORTA    5193
#define CONEXOES 2
#define MAP_SIZE 25

void loadMap();
void gameLoop();
void trataConexao(int playerId);

void config();
void printMap(int mapa[][MAP_SIZE]);
void copyMatrix(int a[][MAP_SIZE], int b[][MAP_SIZE]);
void enqueueNeighbors(Queue *q, Node *n, int mapa[][MAP_SIZE]);
void smallestNeighbor(int mapa[][MAP_SIZE], int *x, int *y);
void enqueueBestWay(Queue *q, Node *s, Node *t, int mapa[][MAP_SIZE]);
void calculateBestMove(Queue *a, Queue *b, int mapa[][MAP_SIZE]);
bool calculateDistance(Monster *m, Player *p1, Player *p2);

int sair = 0;
int st_andamento = 0;

char gameMapNumber;
int mapa[MAP_SIZE][MAP_SIZE];

int playerAssist[2];
int playerDirection[2];
bool playerOneSpecial = false;
bool playerTwoSpecial = false;

bool playerOne = 0;
bool playerTwo = 0;

int monsterBaseSpeed = 750;
int playerBaseSpeed = 500;

int mySocket[2];
HANDLE semMutex;

int main(int argc, char *argv[]) {
// int main() {
    int playerCounter = 0;
    int desc_socket, newsocket;

    HANDLE handle[2];
    DWORD id[2];
    semMutex = CreateMutex(NULL,0,NULL);

    struct sockaddr_in endereco;
    struct sockaddr_in endereco_cliente;
    int size_endereco_cliente;

    config();

    printf("Iniciando WinSock API no servidor...\n");
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(1, 1), &wsaData)!= 0) {
        printf("Servidor: WINSOCK.DLL nao encontrado \n");
        exit( 1 );
    }

    if ((desc_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cliente: errro na criacao do socket \n");
        exit(1);
    }
    endereco.sin_family      = AF_INET;
    endereco.sin_port        = htons(PORTA); // converte num da porta
    endereco.sin_addr.s_addr = inet_addr(ADDRESS);

    if (bind(desc_socket, (struct sockaddr *) &endereco, sizeof(endereco)) < 0) {
        perror("Servidor: erro no bind");
        exit(1);
	}
    printf("\n[Servidor] Servidor no ar...\n\n");

    if (listen(desc_socket, CONEXOES) < 0) {
        perror("[Servidor] erro no listen");
        exit(1);
    }

    while (!sair) {
        size_endereco_cliente = sizeof(struct sockaddr_in);
        newsocket = accept(desc_socket, (struct sockaddr *) &endereco_cliente, &size_endereco_cliente);
        if (newsocket < 0) {
	        perror("[Servidor] erro no accept");
	        exit(1);
	    }

        mySocket[playerCounter] = newsocket;
	    CreateThread(NULL, 0, (void*) trataConexao, playerCounter, 0, &id[playerCounter]);
	    playerCounter++;
    }

    close(desc_socket);

    exit(0);
}

void trataConexao(int playerId) {
    char buffer[1024];
    int playerSocket, n, i, j, k;

    bool endGame = false;
    bool isCharacterSelecion = true;
    bool isGameRunning = false;

    // Pega o socket deste cliente.
    playerSocket = mySocket[playerId];

    // Só tratamos o jogador 1 e o jogador 2, por enquanto.
    if (playerId > 1) {
        close(playerSocket);
        printf("\nIgnorando usuario %d, servidor está cheio.\n", playerId + 1);
        return;
    }

    printf("\nConectado usuario %d\n", playerId + 1);

    while (!endGame) {
        if (isCharacterSelecion) {
            // Se for o primeiro cliente a se conectar
            if (playerId == 0) {
                printf("Requisitando dados do jogador 1\n");
                buffer[0] = ID_CHOOSE_PLAYER_1;
                buffer[1] = '\0';
                send(playerSocket, buffer, strlen(buffer)+1, 0);

                // Aguarda o retorno dos dados
                printf("Aguardando dados do jogador 1\n");
                n = recv(playerSocket, buffer, 100, 0);

                if (buffer[0] == ID_PLAYER_READY) {
                    // Seta as variáveis de ajudante e mapa.
                    WaitForSingleObject(semMutex,INFINITE);
                    playerAssist[0] = buffer[1] - '0';
                    gameMapNumber = buffer[2];

                    // Player 1 está ok.
                    playerOne = true;
                    ReleaseMutex(semMutex);

                    printf("Carregando mapa...\n");
                    loadMap();

                    // Acabou a seleção de personagens.
                    isCharacterSelecion = false;
                } else {
                    printf("Dados invalidos, envie de novo.\n");
                }
            } else {
                // Aguardamos o jogador 1 ficar de boas.
                while (!playerOne);

                printf("Requisitando dados do jogador 2\n");
                buffer[0] = ID_CHOOSE_PLAYER_2;
                buffer[1] = '\0';
                send(playerSocket, buffer, strlen(buffer)+1, 0);

                // Aguarda o retorno dos dados
                printf("Aguardando dados do jogador 2\n");
                n = recv(playerSocket, buffer, 100, 0);

                if (buffer[0] == ID_PLAYER_READY) {
                    // Seta as variáveis de ajudante e mapa.
                    WaitForSingleObject(semMutex,INFINITE);
                    playerAssist[1] = buffer[1] - '0';

                    // Player 2 está ok.
                    playerTwo = true;
                    ReleaseMutex(semMutex);

                    // Acabou a seleção de personagens.
                    isCharacterSelecion = false;
                }
            }
        }

        // Aguarda o ok dos dois jogadores.
        while (!playerOne || !playerTwo);

        // Envia os dados de início de jogo e ajudante do inimigo ao jogador.
        buffer[0] = ID_GAME_START;
        buffer[1] = playerAssist[1-playerId];
        buffer[2] = gameMapNumber;
        buffer[3] = '\0';
        send(playerSocket, buffer, 4, 0);
        isGameRunning = true;

        // Thread do player 1 inicia a Thread de jogo
        if (playerId == 0) {
            CreateThread(NULL, 0, (void*) gameLoop, NULL, 0, NULL);
        }

        while (isGameRunning) {
            n = recv(playerSocket, buffer, 100, 0);
            printf("Received: %d %s\n", playerId, buffer);

            if (buffer[0] == ID_PLAYER_DIRECTION) {
                WaitForSingleObject(semMutex, INFINITE);
                playerDirection[playerId] = buffer[1];
                ReleaseMutex(semMutex);
            } else if (buffer[0] == ID_REQUEST_MAP) {
                WaitForSingleObject(semMutex, INFINITE);
                k = 0;
                buffer[k++] = ID_MAP;

                for (i = 0; i < MAP_SIZE; i++) {
                    for (j = 0; j < MAP_SIZE; j++) {
                        buffer[k++] = mapa[i][j];
                    }
                }

                buffer[k] = '\0';

                send(playerSocket, buffer, strlen(buffer)+1, 0);
                // printf("Sent: %d %s\n", strlen(buffer)+1, buffer);
                ReleaseMutex(semMutex);
            }
        }
    }

    close(playerSocket);
}

void gameLoop() {
    bool gameRunning = true;

    Queue *astar = queue_create();
    Queue *way = queue_create();

    bool playerWalk = true;
    bool monsterWalk = false;

    int intendedPos;
    char buffer[1024];
    buffer[1023] = '\0';

    int t1, t2, delay, timeCounter, framesPassed;
    timeCounter = 0;
    delay = 34; //- 10000/340 ~= 30 FPS
    framesPassed = 0;
    t1 = SDL_GetTicks();

    MonsterList *ml = monster_create();

    PlayerList *pl = player_create();
    player_add(pl, 0, 0, 0, playerBaseSpeed, 0, playerAssist[0], "Sasha");
    player_add(pl, 1, 24, 24, playerBaseSpeed, 0, playerAssist[1], "Jynx");

    mapa[0][0] = PLAYER_ONE;
    mapa[24][24] = PLAYER_TWO;

    // Player *p = pl->first;
    // bool special = false;
    while (gameRunning) {
        Player *p = pl->first;

        WaitForSingleObject(semMutex, INFINITE);
        printf("Parse player walk\n");
        while (p) {
            int playerWeight = (p->id == 0) ? PLAYER_ONE : PLAYER_TWO;
            playerWalk = (framesPassed % p->speed) == 0;

            if (playerWalk) {
                printf("Player can walk\n");

                switch (playerDirection[p->id]) {
                case DOWN:
                    if (p->y < (MAP_SIZE-1)) {
                        intendedPos = mapa[p->x][p->y+1];

                        if (intendedPos == FLOOR || intendedPos == SPECIAL_ITEM) {
                            mapa[p->x][p->y] = FLOOR;
                            p->y++;
                        }
                    }
                    break;

                case UP:
                    if (p->y > 0) {
                        intendedPos = mapa[p->x][p->y-1];

                        if (intendedPos == FLOOR || intendedPos == SPECIAL_ITEM) {
                            mapa[p->x][p->y] = FLOOR;
                            p->y--;
                        }
                    }
                    break;

                case RIGHT:
                    if (p->x < (MAP_SIZE-1)) {
                        intendedPos = mapa[p->x+1][p->y];

                        if (intendedPos == FLOOR || intendedPos == SPECIAL_ITEM) {
                            mapa[p->x][p->y] = FLOOR;
                            p->x++;
                        }
                    }
                    break;

                case LEFT:
                    if (p->x > 0) {
                        intendedPos = mapa[p->x-1][p->y];

                        if (intendedPos == FLOOR || intendedPos == SPECIAL_ITEM) {
                            mapa[p->x][p->y] = FLOOR;
                            p->x--;
                        }
                    }
                    break;

                // default:
                }

                mapa[p->x][p->y] = playerWeight;
            }

            p = p-> next;
        } // Fim while(p)

        Monster *m = ml->first;
        while (m) {
            monsterWalk = (framesPassed % m->speed) == 0;

            if (monsterWalk) {
                int px, py;
                bool playerOneIsCloser;

                Player *p1 = pl->first;
                Player *p2 = p1->next;

                mapa[m->x][m->y] = FLOOR;

                // todo get euclidian distance.

                queue_clear(astar);
                queue_clear(way);
                queue_enq(astar, m->x, m->y, 0);

                playerOneIsCloser = calculateDistance(m, p1, p2);
                px = playerOneIsCloser ? p1->x : p2->x;
                py = playerOneIsCloser ? p1->y : p2->y;
                queue_enq(astar, px, py, 0);

                calculateBestMove(astar, way, mapa);

                if (!queue_isEmpty(way)) {
                    Node *n = queue_deq(way);
                    m->x = n->x;
                    m->y = n->y;

                    // printf("Node: ");
                    // queue_printNode(n);
                }

                mapa[m->x][m->y] = MONSTER_BLUE;

                if (m->x == p1->x && m->y == p1->y) {
                    // TODO fim de jogo p1
                    buffer[0] = ID_GAME_END;
                    buffer[1] = GAME_DEFEAT;
                    buffer[2] = '\0';
                    send(mySocket[0], buffer, strlen(buffer)+1, 0);

                    buffer[1] = GAME_VICTORY;
                    send(mySocket[1], buffer, strlen(buffer)+1, 0);

                    gameRunning = false;
                    ReleaseMutex(semMutex);
                } else if (m->x == p2->x && m->y == p2->y) {
                    // TODO fim de jogo p2
                    buffer[0] = ID_GAME_END;
                    buffer[1] = GAME_DEFEAT;
                    buffer[2] = '\0';
                    send(mySocket[1], buffer, strlen(buffer)+1, 0);

                    buffer[1] = GAME_VICTORY;
                    send(mySocket[0], buffer, strlen(buffer)+1, 0);

                    gameRunning = false;
                    ReleaseMutex(semMutex);
                }
            }

            m = m->next;
        } // Fim while (m)
        ReleaseMutex(semMutex);

        // Conta mais um frame passado.
        framesPassed = (framesPassed++) % (10000);

        // Calcula os milissegundos passados.
        t2 = SDL_GetTicks() - t1;

        // Espera o resto do tempo pra completar 1 frame
        if (t2 < delay){
            SDL_Delay(delay - t2);
        }

        if (t2 > 1000) {
            t1 = SDL_GetTicks();

            if ((timeCounter % 15) == 0) {
                monsterBaseSpeed -= 5;
                monster_add(ml, 12, 12, 0, monsterBaseSpeed, 0);
            }

            timeCounter++;
        }
    } // fim while(gameRunning)

    queue_free(astar);
    queue_free(way);
    monster_free(ml);
    player_free(pl);
}

void config() {
    FILE *configFile;
    int data;

    configFile = fopen("config.txt", "r");
    if (!configFile) {
        printf("Erro ao abrir arquivo de configuracao!\n");
        exit(1);
    }

    fscanf(configFile, "%d", &playerBaseSpeed);
    fscanf(configFile, "%d", &monsterBaseSpeed);

    fclose(configFile);
}

void loadMap() {
    FILE *mapFile;
    int square;
    int i, j;

    char c_mapa[] = "mapa0.xxxmap";
    c_mapa[4] = gameMapNumber;

    printf("\n\nMapa: %s\n\n", c_mapa);

    mapFile = fopen(c_mapa, "r");
    if (!mapFile) {
        printf("Erro ao carregar o mapa!");
        exit(1);
    }

    for (j = 0; j < MAP_SIZE; j++) {
        for (i = 0; i < MAP_SIZE; i++) {
            fscanf(mapFile, "%d", &square);
            // printf("%d ", square);

            mapa[i][j] = (square == -1) ? OBSTACLE : FLOOR;
        }
        // printf("\n");
    }

    printMap(mapa);
}

void printMap(int mapa[][MAP_SIZE]) {
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
    int i, j, aux;

    for (i = 0; i < MAP_SIZE; i++) {
        for (j = 0; j < MAP_SIZE; j++) {
            aux = a[i][j];

            if (aux == PLAYER_ONE || aux == PLAYER_TWO) {
                aux = -2;
            } else if (aux == FLOOR) {
                aux = 0;
            } else {
                aux = -1;
            }

            b[i][j] = aux;
        }
    }
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
    /*
        Inicialmente a intenção era dar toda a lista de possibilidades de movimentação
        mas agora nós damos apenas 1 nó de movimentação por vez, por questões de otimização.
    */

    int xs = s->x, ys = s->y;
    // int xt = t->x, yt = t->y;
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

void calculateBestMove(Queue *a, Queue *b, int mapa[][MAP_SIZE]) {
    Node *monster = queue_deq(a);
    Node *player = queue_peek(a);

    int mapaAux[MAP_SIZE][MAP_SIZE];
    copyMatrix(mapa, mapaAux);

    // printMap(mapaAux);

    while (!queue_isEmpty(a))
        enqueueNeighbors(a, queue_deq(a), mapaAux);

    // printMap(mapaAux);

    // return;
    // printf("player: ");
    // queue_printNode(player);
    // printf("monster: ");
    // queue_printNode(monster);
    enqueueBestWay(b, monster, player, mapaAux);
    // queue_print(b);
}

bool calculateDistance(Monster *m, Player *p1, Player *p2) {
    int mx = m->x, my = m->y;
    int p1x = p1->x, p1y = p1->y;
    int p2x = p2->x, p2y = p2->y;

    long dist1 = ((mx - p1x) * (mx - p1x)) + ((my - p1y) * (my - p1y));
    long dist2 = ((mx - p2x) * (mx - p2x)) + ((my - p2y) * (my - p2y));

    return (dist1 <= dist2);
}
