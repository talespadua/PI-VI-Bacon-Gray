#include <stdio.h>
#include <winsock.h>
#include <ctype.h>

#include "utils.h"

#define PORTA   5193

int num_jogador;
char address[16];

int isPlayerOne = 0;
int opponentAssit;

void inicio(int *socket_cliente) {
    char buffer[627];
    int aux = 0;

    printf("Aguardando instruções do servidor...\n");
    recv(socket_cliente, buffer, 100, 0);
    printf("Recebido: |%s|\n", buffer);

    if (buffer[0] == ID_CHOOSE_PLAYER_1) {
        isPlayerOne = 1;
    }

    printf("Choose your destiny:\n");
    scanf("%s", buffer);
    buffer[2] = isPlayerOne ? buffer[1] : '0';
    buffer[1] = buffer[0];
    buffer[0] = ID_PLAYER_READY;

    send(socket_cliente, buffer, strlen(buffer)+1, 0);

    recv(socket_cliente, buffer, 100, 0);
    printf("\nRecebido: |%s|", buffer);

    while (strcmp(buffer, "end")) {
        recv(socket_cliente, buffer, 627, 0);
        printf("\n\nRecebido:\n%s", buffer);
    }
}

void jogo(int *socket_cliente){

}

void config() {
    FILE *configFile;
    int flag = 0;
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

        address[i++] = data;
    }

    address[i] = '\0';
    fclose(configFile);
}

int main(void) {
    int desc_socket;
    struct sockaddr_in endereco;

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
    jogo(desc_socket);

    system("pause");
    exit(0);
}
