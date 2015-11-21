#include <stdio.h>
#include <winsock.h>
#define ADDRESS  "127.0.0.1"
#define PORTA    5193
#define CONEXOES 2

int sair = 0;
int contador = 0;

int jg_jogador[2];
int jg_ajudante[2];
int jg_mapa;

int st_andamento = 0;

HANDLE semMutex;

void trataConexao(int *socket_cliente){
    char buffer[512];
    int cs,n;

    int local_sair = 0;
    int local_contador = 0;

    int inicio = 0;
    int aux = 0;
    char c_aux[2];

    WaitForSingleObject(semMutex,INFINITE);
    local_contador = contador;
    contador++;
    printf("\nConectado usuario %d", local_contador + 1);
    ReleaseMutex(semMutex);

    cs = *socket_cliente; // guarda copia local.
    while(!local_sair){
        if(!inicio){
            WaitForSingleObject(semMutex,INFINITE);
            aux = st_andamento;
            ReleaseMutex(semMutex);

            //Se for o primeiro cliente a se conectar
            if(local_contador == 0){
                //Envia um OK para o cliente
                printf("\nEnviando OK para o jogador 1");
                strcpy(buffer,"ok ");
                send(cs, buffer, strlen(buffer)+1,0);

                //Muda o status do andamento
                WaitForSingleObject(semMutex,INFINITE);
                st_andamento = 1;
                ReleaseMutex(semMutex);

                //Aguarda o retorno dos dados
                printf("\nAguardando dados do jogador 1");
                n = recv(cs, buffer,100,0);

                //Muda o status do andamento e atualiza os dados do primeiro jogador
                WaitForSingleObject(semMutex,INFINITE);
                jg_jogador[local_contador] = buffer[0] - '0';
                jg_ajudante[local_contador] = buffer[1] - '0';
                st_andamento = 2;

                /*printf("\n> jogador 1 escolheu jogador:  %d", jg_jogador[local_contador]);
                printf("\n> jogador 1 escolheu ajudante: %d", jg_ajudante[local_contador]);*/
                ReleaseMutex(semMutex);

                printf("\nAguardando jogador 2...");
            }

            if(local_contador == 1){
                printf("\nAguardando jogador 1...");
                while(aux != 2)
                {
                    WaitForSingleObject(semMutex,INFINITE);
                    aux = st_andamento;
                    ReleaseMutex(semMutex);
                }

                WaitForSingleObject(semMutex,INFINITE);
                aux = jg_jogador[0];
                ReleaseMutex(semMutex);

                //Envia um OK para o cliente
                printf("\nEnviando OK para o jogador 2");
                strcpy(buffer, "ok");
                sprintf(c_aux, "%d", aux);
                strcat(buffer, c_aux);
                send(cs, buffer, strlen(buffer)+1,0);

                //Muda o status do andamento
                WaitForSingleObject(semMutex,INFINITE);
                st_andamento = 3;
                ReleaseMutex(semMutex);

                //Aguarda o retorno dos dados
                printf("\nAguardando dados do jogador 2");
                n = recv(cs, buffer,100,0);

                //Muda o status do andamento e atualiza os dados do segundo jogador
                WaitForSingleObject(semMutex,INFINITE);
                if(buffer[0])
                jg_jogador[local_contador] = buffer[0];
                jg_ajudante[local_contador] = buffer[1];
                jg_mapa = buffer[2];
                st_andamento = 4;
                ReleaseMutex(semMutex);
            }
        }

        while(aux != 4)
        {
            WaitForSingleObject(semMutex,INFINITE);
            aux = st_andamento;
            ReleaseMutex(semMutex);
        }

        strcpy(buffer,"ini");
        send(cs, buffer, strlen(buffer)+1,0);
    }
    close(cs);
}

int main(void)
{
    int desc_socket, newsocket;

    HANDLE handle[2];
    DWORD id[2];
    semMutex = CreateMutex(NULL,FALSE,NULL);

    struct sockaddr_in endereco;
    struct sockaddr_in endereco_cliente;
    int size_endereco_cliente;

    printf( "Iniciando WinSock API no servidor...\n" );
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(1, 1), &wsaData)!= 0){
        printf("Servidor: WINSOCK.DLL nao encontrado \n" );
        exit( 1 );
    }

    if ((desc_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cliente: errro na criacao do socket \n");
        exit(1);
    }
    endereco.sin_family      = AF_INET;
    endereco.sin_port        = htons(PORTA); // converte num da porta
    endereco.sin_addr.s_addr = inet_addr(ADDRESS);

    if(bind(desc_socket, (struct sockaddr *)&endereco, sizeof(endereco)) < 0){
        perror("Servidor: erro no bind");
        exit(1);
	}
    printf("\n[Servidor] Servidor no ar...\n\n");

    if (listen(desc_socket, CONEXOES) < 0) {
        perror("[Servidor] erro no listen");
        exit(1);
    }

    while (!sair)
    {
        size_endereco_cliente = sizeof(struct sockaddr_in);
        newsocket = accept(desc_socket, (struct sockaddr *) &endereco_cliente, &size_endereco_cliente);
        if(newsocket < 0){
	        perror("[Servidor] erro no accept");
	        exit(1);
	    }

	    WaitForSingleObject(semMutex,INFINITE);
	    CreateThread(NULL, 0, (void*)trataConexao, &newsocket, 0, &id[contador]);
	    ReleaseMutex(semMutex);
    }

    close(desc_socket);

    exit(0);
}
