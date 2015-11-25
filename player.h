#ifndef SASHA_PLAYER_H
#define SASHA_PLAYER_H

#include <stdlib.h>
#include <stdio.h>

#define PLAYER_NAME_SIZE 11
#define PLAYER_BASE_SPEED 12

typedef struct Player Player;
typedef struct PlayerList PlayerList;

struct Player {
    int id; // Número identificador do jogador.
    int x, y; // Localização x e y.
    int speed; // Velocidade de movimento em FPS.
    int condition; // Condição do jogador. 0 para normal, outro valor para algum status especial (ex. cegueira, lentidão). // TODO fazer um enum
    int character; // Personagem escolhido para jogar. // TODO fazer um enum
    int special; // Quantidade de especiais acumulados pelo jogador.
    char name[NAME_SIZE]; // Nome do jogador (até 10 caracteres). (Opcional, fica como melhoria).

    Player *next; // Próximo jogador da lista.
};

struct PlayerList {
    int size; // Tamanho da lista.
    Player *first; // Jogador no início da lista.
    Player *last; // Jogador no fim da lista.
};

PlayerList* player_create(); // Cria uma nova lista.
void player_clear(PlayerList *pl); // Limpa todos os jogadores da lista.
void player_add(PlayerList *pl, int id, int x, int y, int condition, int character, char name[NAME_SIZE]); // Adiciona um novo jogador na lista.
Player* player_peek(PlayerList *pl); // Retorna o primeiro jogador da lista sem removê-lo.
Player* player_remove(PlayerList *pl, Player *p); // Remove um jogador específico da lista.
int player_isEmpty(PlayerList *pl); // Verifica se a lista está vazia.
void player_print(player *pl); // Printa os ids de todos os jogadores na lista.
void player_printFull(player *pl); // Printa todas as informações de todos os jogadores na lista.

PlayerList* player_create() {
    PlayerList *pl = (PlayerList*) malloc(sizeof(PlayerList));
    pl->size = 0;
    pl->first = pl->last = NULL;

    return pl;
}

void player_clear(PlayerList *pl) {
    while (pl->first != NULL) {
        Player *temp = pl->first;
        pl->first = temp->next;

        // printf("Removing: ");
        // player_printPlayer(temp);
        free(temp);
    }

    pl->size = 0;
    pl->last = NULL;
}

void player_add(PlayerList *pl, int id, int x, int y, int condition, int character, char name[NAME_SIZE]) {
    Player *p = (Player*) malloc(sizeof(Player));
    p->id = id;
    p->x = x;
    p->y = y;
    p->speed = PLAYER_BASE_SPEED;
    p->condition = condition;
    p->character = character;
    p->special = 0;
    p->next = NULL;

    int i;
    for (i = 0; i < NAME_SIZE; i++) {
        p->name[i] = name[i];
    }

    if (player_isEmpty(pl)) {
        pl->first = pl->last = p;
    } else {
        p->last->next = p;
        p->last = p;
    }

    pl->size++;
}

Player* player_remove(PlayerList *pl, Player *p) {
    if (player_isEmpty(pl)) {
        printf("Lista vazia!\n");
        return NULL;
    }

    Player *temp = pl->first;
    Player *before = NULL;
    while (temp) {
        if (temp->id == p->id) {
            // Estamos removendo do meio da lista, vamos assegurar que ela permanece contínua.
            if (before) {
                before->next = temp->next;
            }
            pl->size--;

            if (player_isEmpty(pl)) {
                pl->last = pl->first;
            }
        } else {
            before = temp;
            temp = temp->next;
        }
    }

    printf("Jogador nao encontrado!\n");
}

Player* player_peek(PlayerList *pl) {
    return pl->first;
}

int player_isEmpty(PlayerList *pl) {
    return (pl->size == 0);
}

void player_print(PlayerList *pl) {
    Player *temp = pl->first;

    while(temp != NULL) {
        printf("id: %d, ", temp->id);
        temp = temp->next;
    }
}

void player_printFull(PlayerList *pl) {
    Player *temp = pl->first;

    while(temp != NULL) {
        printf("id: %d, name: %s, x: %d, y: %d, speed: %d, condition: %d, character: %d, special: %d \n", temp->id, temp->name, temp->x, temp->y, temp->speed, temp->condition, temp->character, temp->special);
        temp = temp->next;
    }

    // printf("Fim da fila!\n\n");
}

#endif // SASHA_PLAYER_H
