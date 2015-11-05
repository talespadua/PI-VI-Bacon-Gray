/*------------------------------------------------------------
 * Programação com Sockets usando a winsock no Codeblocks
 * Programa: servidorTCP.c
 *           codigo do programa servidor utilizando TCP
 *
 * Objetivo: alocar um socket, conectar ao servidor e enviar
 *		     uma mensagem de ola e aguardar a resposta do servidor.
 *
 *
 * Dicas para incluir a lib winsock no Codeblocks
 *
 * 1) No menu “Settings” -> "Compiler", na aba “Linker settings”, clique em “Add”.
 * 2) Escolha a seguinte lib "C:\Arquivos de Programas\CodeBlocks\MinGW\lib\libws2_32.a"
 * 3) Compile e execute cada executável a partir de um terminal diferente (janela de DOS), iniciando primeiramente o servidorTCP.
 *
 * Autor...: Fabio Lubacheski
 *----------------------------------------------------------
*/

#include <stdio.h>
#include <winsock.h> //Biblioteca para se programar em sockets
#define ADDRESS     "127.0.0.1" // coloque aqui o IP do servidor
#define PORTA       5193        // coloque aqui o numero da porta do servidor
#define CONEXOES    5           // numero de conexoes simultaneas

int sair = 0;
int contador = 0;
HANDLE semMutex;

// funcao poderia ser uma thread que trata a nova conexao
void trataConexao(int *socket_cliente){
    char buffer[512];
    int cs,n;
    int sair_funcao = 0;
    int contador_local = 0;

    WaitForSingleObject(semMutex,INFINITE);
    contador++;
    contador_local = contador;
    printf("\nConectado usuario %d", contador);
    ReleaseMutex(semMutex);

    cs = *socket_cliente; // guarda copia local.
    while(!sair_funcao){
        n = recv(cs,buffer,100,0);

        if(strcmp(buffer,"quit")==0){
            sair_funcao = 1;

            WaitForSingleObject(semMutex,INFINITE);
            contador--;
            if(contador == 0){
                sair = 1;
                printf("\nNao ha mais clientes conectados");
            }
            ReleaseMutex(semMutex);
        }
        else{
            printf("\nCliente %d enviou %d bytes. Msg: %s ", contador_local, n, buffer);
            fflush(stdout);
        }

        strcpy(buffer,"ok");
        send(cs,buffer, strlen(buffer)+1,0);
    }
    close(cs);
}

int main(void)
{
    int  desc_socket, // descritor do socket do servidor
         newsocket;   // descritor do socket do cliente

    HANDLE handle[2];
    DWORD id[2]; // identificador das threads
    semMutex = CreateMutex(NULL,FALSE,NULL);

    struct sockaddr_in endereco;  // endereco do servidor (ip+porta)
    struct sockaddr_in endereco_cliente; // endereco do cliente  (ip+porta)
    int size_endereco_cliente;            // comprimento do endereço do cliente

    /*
    Todo programa que use a Windows Sockets DLL deve fazer uma chamada da
    funcao WSAStartup, que verifica a versao da Windows Sockets DLL instalada no
    computador, cuja sintaxe segue logo abaixo:

    Sintaxe: int WSAStartup (WORD wVersionRequested,LPWSADATA lpWSAData);
    os parametros sao:
    WORD wVersionRequested: eh o numero da versao do Winsocks exigido pelo
      seu programa. As versoes mais comuns sao 1.0, 1.1 e 2.0.
      wVersionRequested = MAKEWORD(1,1);
      Isso exigirá que o computador tenha a versao 1.1 da dll winsocks.

    LPWSADATA lpWSAData: eh um ponteiro para uma estrutura WSADATA

    */
    printf( "Iniciando WinSock API no servidor...\n" );
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(1, 1), &wsaData)!= 0)
    {
        printf("Servidor: WINSOCK.DLL nao encontrado \n" );
        exit( 1 );
    }
    /*
    Criacao de um Socket

    para UDP use SOCK_DGRAM e para TCP use SOCK_STREAM


    socket(familia, tipo do protocolo, número do protocolo)

    Esta funcao cria o socket a ser usado numa das "pontas" da comunicacao,
    Os parametros sao os seguintes:

    * familia: especifica a familia de protocolos de rede a ser utilizado. Os valores
      podem ser:
      AF_INET (Arpa Internet Protocols)
      AF_UNIX (Unix Internet Protocols)
      AF_ISSO (Iso Protocols)
      AF_NS (Xerox Network System Protocols)
      Os nossos exemplos irão se restringir aos protocolos "internet", especificados por AF_INET.
    * tipo do protocolo: define a categoria do  protocolo stream (SOCK_STREAM) ou datagrama (SOCK_DGRAM).
    * número do protocolo: indica o protocolo a ser usado, dentro da familia.
       0 – IP – Internet Protocol
       1 – ICMP – Internet Control Message Protocol
       2 – IGMP – Internet Group Multicast Protocol
       3 – GGP – Gateway-Gateway Protocol
       6 – TCP – Transmission Control Protocol
      17 – UDP – User Datagram Protocol

    Essa funcao retorna um inteiro correspondente ao descritor do socket criado ou o valor -1,
    indicando que o socket nao o pode ser criado por alguma razao.
    */
    if ((desc_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cliente: errro na criacao do socket \n");
        exit(1);
    }
    /*
      A funcao bind associa uma porta no computador local para o socket criado.
      Essa funcao eh muito usada com outra funcao, a listen(),

      As informacoes adicionais sao:

      No campo endereco.sin_family eh informado que o endereco do servidor eh da familia
      AF_INET, ou seja um numero IP com 4 decimais separados por pontos (Ex 192.168.0.2)

      Na variavel endereco.sin_port o numero da porta eh convertido para representacao
      big endian, ou seja, ordem dos bytes da rede (network byte order) com o byte mais
      primeiro.

      a funcao htons converte o numero inteiro do ordem de bytes do host para representacao
      da rede

      A funcao inet_addr converte um ip no formato dotted-decimal (ou seja, xxx.xxx.xxx.xxx)
      para in_addr
    */
    endereco.sin_family      = AF_INET;
    endereco.sin_port        = htons(PORTA); // converte num da porta
    endereco.sin_addr.s_addr = inet_addr(ADDRESS);

    if(bind(desc_socket, (struct sockaddr *)&endereco, sizeof(endereco)) < 0){
        perror("Servidor: erro no bind");
        exit(1);
	}
    printf("\n[Servidor] Servidor no ar...\n\n");


    /*
     A funcao listen coloca o programa em modo listen, ou seja, espera uma determinada conexão em um socket.
     Além disso ela limita o numero de conexoes simultaneas, no nosso caso limitamos em 5 conexoes ao mesmo tempo.
    */
    if (listen(desc_socket, CONEXOES) < 0) {
        perror("[Servidor] erro no listen");
        exit(1);
    }

    while (!sair)
    {
        /*
         A funcao accept espera e aceita (ou não) um pedido de conexao do cliente
         Cria um novo descritor de socket para tratar a conexao cliente
         newsocket = socket do cliente

        */

        //perror("\nAguardando cliente...");
        size_endereco_cliente = sizeof(struct sockaddr_in);
        newsocket = accept(desc_socket, (struct sockaddr *) &endereco_cliente, &size_endereco_cliente);
        if(newsocket < 0){
	        perror("[Servidor] erro no accept");
	        exit(1);
	    }

	    WaitForSingleObject(semMutex,INFINITE);
	    //pthread_create(&id[contador], NULL, (void*)trataConexao,  &newsocket);
	    CreateThread(NULL, 0, (void*)trataConexao, &newsocket, 0, &id[contador]);
	    ReleaseMutex(semMutex);
        //trataConexao(&newsocket);
    }
    // loop infinito. Como derrubar servidor?

    close(desc_socket);

    exit(0);
}




/*
// cria nova thread para tratar a conexao
//pthread_create( &id, NULL, (void*)trata,  &newsocket);
*/
