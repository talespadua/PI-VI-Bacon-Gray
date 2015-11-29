#include <stdio.h>
#include <winsock.h>
#include "utils.h"

#define ADDRESS  "0.0.0.0"
#define PORTA    5193
#define CONEXOES 2
#define MAP_SIZE 25

int sair = 0;

int playerAssist[2];
char gameMapNumber;

int st_andamento = 0;

HANDLE semMutex;

int mapa[25][25];

int mySocket[2];

int playerOne = 0;
int playerTwo = 0;

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
            printf("%d ", square);
            mapa[i][j] = square;
        }

        printf("\n");
    }
}

void trataConexao(int playerId) {
    char buffer[512];
    int playerSocket, n;

    int endGame = 0;
    int isCharacterSelecion = 1;
    int isGameRunning = 0;

    int inicio = 0;
    int aux = 0;

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
                    playerOne = 1;
                    ReleaseMutex(semMutex);

                    printf("Carregando mapa...\n");
                    loadMap();

                    // Acabou a seleção de personagens.
                    isCharacterSelecion = 0;
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
                    gameMapNumber = buffer[2];

                    // Player 2 está ok.
                    playerTwo = 1;
                    ReleaseMutex(semMutex);

                    // Acabou a seleção de personagens.
                    isCharacterSelecion = 0;
                }
            }
        }

        // Aguarda o ok dos dois jogadores.
        while (!playerOne || !playerTwo);
        printf("That\'s good, that\'s alright\n");

        // Envia os dados de início de jogo e ajudante do inimigo ao jogador.
        buffer[0] = ID_GAME_START;
        buffer[1] = playerAssist[1-playerId];
        buffer[2] = '\0';
        send(playerSocket, buffer, strlen(buffer)+1, 0);
        isGameRunning = 1;

        while (isGameRunning) {

        }
    }

    close(playerSocket);
}

void broadcast(){
    int aux, fim;
    int i, j, k;
    int vetor[627];
    vetor[626] = '\0';

    while(aux != 4) {
        WaitForSingleObject(semMutex,INFINITE);
        aux = st_andamento;
        ReleaseMutex(semMutex);
    }

    while(fim) {
        //vetor[0] = abacaxi_voador_azul_de_asas;
        k = 1;
        for (j = 0; j < 25; j++) {
            for (i = 0; i < 25; i++) {
                WaitForSingleObject(semMutex,INFINITE);
                vetor[k++] = mapa[i][j];
                ReleaseMutex(semMutex);
            }
        }
        send(mySocket[0], vetor, strlen(vetor)+1,0);
        send(mySocket[1], vetor, strlen(vetor)+1,0);
    }
}

int main(void) {
    int playerCounter = 0;
    int desc_socket, newsocket;

    HANDLE handle[2];
    DWORD id[2];
    semMutex = CreateMutex(NULL,0,NULL);

    struct sockaddr_in endereco;
    struct sockaddr_in endereco_cliente;
    int size_endereco_cliente;

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
