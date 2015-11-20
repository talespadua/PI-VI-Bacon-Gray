#include <stdio.h>
#include <winsock.h>

#define ADDRESS "10.135.117.20"
#define PORTA   5193

void inicio(int *socket_cliente){
    char buffer[512];
    int aux = 0;
    int cs,n;

    printf("\nAguardando OK do servidor...");
    recv(desc_socket, buffer, 100, 0);

    if()
}

int main( void )
{
    int desc_socket;
    struct sockaddr_in endereco;

    char buffer[512];
    int sair = 0;

    printf( "Iniciando WinSock API...\n" );
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(1, 1), &wsaData)!= 0){
        printf("Cliente: WINSOCK.DLL nao encontrado \n" );
        exit( 1 );
    }

    if ((desc_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cliente: errro na criacao do socket \n");
        exit(1);
    }

    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(PORTA);
    endereco.sin_addr.s_addr = inet_addr(ADDRESS);

    if(connect(desc_socket, (struct sockaddr *) &endereco, sizeof(endereco)) < 0) {
        printf("Cliente: erro na conexao \n");
        exit(1);
    }

    inicio(&desc_socket);

    while(!sair)
    {
        printf("\nDigite um mensagem (\"quit\"  encerra):");
        scanf("%s", buffer);
        send(desc_socket, buffer, strlen(buffer)+1, 0);
        if (strcmp(buffer, "quit")==0)
	        break;


	    printf("\nAguardando retorno...");
	    recv(desc_socket, buffer, 100, 0);
	    printf ("\nServidor respondeu com %s",  buffer);
    }
    close(desc_socket);

    exit(0);
}
