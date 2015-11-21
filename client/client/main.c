#include <stdio.h>
#include <winsock.h>

#define ADDRESS "127.0.0.1"
#define PORTA   5193

int num_jogador;
int porta[5];
char address[15];

void inicio(int *socket_cliente){
    char buffer[512];
    int aux = 0;

    printf("\nAguardando OK do servidor...");
    recv(socket_cliente, buffer, 100, 0);
    printf("\nRecebido: |%s|", buffer);

    if(buffer[2] == ' '){
        num_jogador = 0;
        printf("\nJogador pode escolher qualquer jogador");
        printf("\n[num_jogador][num_ajudante]\n");
    } else {
        num_jogador = 1;
        printf("\nJogador não pode escolher o jogador numero %c", buffer[2]);
        printf("\n[num_jogador][num_ajudante][num_mapa]\n");
    }

    scanf("%s", buffer);
    send(socket_cliente, buffer, strlen(buffer)+1, 0);

    recv(socket_cliente, buffer, 100, 0);
    printf("\nRecebido: |%s|", buffer);

    if(!strcmp(buffer, "ini"))
    {
        printf("\n\nIniciando jogo...\n\n");
    }
}

void jogo(int *socket_cliente){

}

void config(){
    FILE *arquivo;
    int flag = 0;
    char data;
    char aux[2];

    arquivo = fopen("config.txt", "r");
    while((data = fgetc(arquivo)) != EOF){
        *aux = data;
        if(data == '\n')
            flag = 1;
        /*else if(flag == 1)
            flag = 2;
        else if(flag == 2)
            flag = 3;*/
        else{
            if(flag == 0){
                if(address == "")
                    strcpy(address, aux);
                else
                    strcat(address, aux);
            } else {
                if(porta == "")
                    strcpy(porta, aux);
                else
                    strcat(porta, aux);
            }
        }
    }

    printf("Endereco: %s\n", address);
    printf("Porta: %s\n", porta);
}

int main( void )
{
    int desc_socket;
    struct sockaddr_in endereco;

    char buffer[512];
    int sair = 0;

    config();

    printf( "Iniciando WinSock API...\n" );
    WSADATA wsaData;

    if(WSAStartup(MAKEWORD(1, 1), &wsaData)!= 0){
        printf("Cliente: WINSOCK.DLL nao encontrado \n" );
        exit( 1 );
    }

    if((desc_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cliente: errro na criacao do socket \n");
        exit(1);
    }

    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(porta);
    endereco.sin_addr.s_addr = inet_addr(address);

    if(connect(desc_socket, (struct sockaddr *) &endereco, sizeof(endereco)) < 0) {
        printf("Cliente: erro na conexao \n");
        exit(1);
    }


    inicio(desc_socket);
    jogo(desc_socket);

    system("pause");
    exit(0);
}
