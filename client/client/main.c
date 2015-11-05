/*------------------------------------------------------------
 * Programação com Sockets usando a winsock no Codeblocks
 * Programa: clienteTCP.c
 *           codigo do programa cliente utilizando TCP
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

#define ADDRESS     "10.135.117.20"  // coloque aqui o IP do servidor
#define PORTA       5193         // coloque aqui o numero da porta do servidor

int main( void )
{
   int desc_socket; //descritor do socket do cliente
   struct sockaddr_in endereco; // endereco do servidor (ip+porta)

   char buffer[512];
   int sair=0;


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
    printf( "Iniciando WinSock API...\n" );
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(1, 1), &wsaData)!= 0)
    {
        printf("Cliente: WINSOCK.DLL nao encontrado \n" );
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
      A funcao connect estabelece a conexao com o servidor, mas antes deve
      colocar informacoes adicionais na variavel estrutura endereco (struct sockaddr_in).

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

    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(PORTA);
    endereco.sin_addr.s_addr = inet_addr(ADDRESS);
    //memset(endereco.sin_zero,0,8);

    // pede conexao com o servivor
    if(connect(desc_socket, (struct sockaddr *) &endereco, sizeof(endereco)) < 0) {
        printf("Cliente: erro na conexao \n");
        exit(1);
    }

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
