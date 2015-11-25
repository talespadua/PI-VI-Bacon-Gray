#ifndef SASHA_MONSTER_H
#define SASHA_MONSTER_H

#include <stdlib.h>
#include <stdio.h>

#define MONSTER_BASE_SPEED 10

typedef struct Monster Monster;
typedef struct MonsterList MonsterList;

struct Monster {
    int id; // Número de entrada do monstro na lista (tamanho do MonsterList.size na hora que ele entrou).
    int x, y; // Localização x e y.
    int mode; // Modo de perseguição. 0 para seguir o jogador mais próximo ou n para seguir todos menos o jogador com id == n.
    int speed; // Velocidade de movimento em FPS.
    int condition; // Condição do monstro. 0 para normal, outro valor para algum status especial (ex. cegueira, lentidão).
    
    Monster *next; // Próximo monstro da lista.
};

struct MonsterList {
    int size; // Tamanho da lista.
    int count; // Quantidade de monstros que passaram por esta lista.
    Monster *first; // Monstro no início da lista.
    Monster *last; // Monstro no final da lista.
};

MonsterList* monster_create(); // Cria uma nova lista.
void monster_clear(MonsterList *ml); // Limpa todos os monstros da lista.
void monster_add(MonsterList *ml, int x, int y, int mode, int speed, int condition); // Adiciona um novo monstro na lista.
void monster_remove(MonsterList *ml, Monster *m); // Remove um monstro específico da lista.
Monster* monster_peek(MonsterList *ml); // Retorna o primeiro monstro da lista sem removê-lo.
int monster_isEmpty(MonsterList *ml); // Verifica se a lista está vazia.
void monster_print(Monster *ml); // Printa os ids de todos os monstros na lista.
void monster_printFull(Monster *ml); // Printa todas as informações de todos os monstros na lista.

MonsterList* monster_create() {
    MonsterList *ml = (MonsterList*) malloc(sizeof(MonsterList));
    ml->size = 0;
    ml->count = 0;
    ml->first = ml->last = NULL;

    return ml;
}

void monster_clear(MonsterList *ml) {
    while (ml->first != NULL) {
        Monster *temp = ml->first;
        ml->first = temp->next;

        // printf("Removing: ");
        // MonsterList_printMonster(temp);
        free(temp);
    }

    ml->size = 0;
    ml->last = NULL;
}

void monster_add(MonsterList *ml, int x, int y, int mode, int speed, int condition) {
    Monster *m = (Monster*) malloc(sizeof(Monster));
    m->id = ml->count++;
    m->x = x;
    m->y = y;
    m->mode = mode;
    m->speed = speed;
    m->condition = 0;
    m->next = NULL;

    if (monster_isEmpty(ml)) {
        ml->first = ml->last = m;
    } else {
        ml->last->next = m;
        ml->last = m;
    }

    ml->size++;
}

void monster_remove(MonsterList *ml, Monster m) {
    if (monster_isEmpty(ml)) {
        printf("Lista vazia!\n");
        return NULL;
    }

    Monster *temp = ml->first;
    Monster *before = NULL;
    while (temp) {
        if (temp->id == m->id) {
            // Estamos removendo do meio da lista, vamos assegurar que ela permanece contínua.
            if (before) {
                before->next = temp->next;
            }
            ml->size--;

            if (monster_isEmpty(ml)) {
                ml->last = ml->first;
            }
        } else {
            before = temp;
            temp = temp->next;
        }
    }

    printf("Monstro nao encontrado!\n");
}

Monster* monster_peek(MonsterList *ml) {
    return ml->first;
}

int monster_isEmpty(MonsterList *ml) {
    return (ml->size == 0);
}

void monster_print(MonsterList *ml) {    
    Monster *temp = ml->first;

    while (temp != NULL) {
        printf("id: %d ", temp->id);
        temp = temp->next;
    }
}

void monster_printFull(MonsterList *ml) {
    Monster *temp = ml->first;

    while(temp != NULL) {
        printf("id: %d, x: %d, y: %d, mode: %d, speed: %d, condition: %d\n", temp->id, temp->x, temp->y, temp->mode, temp->speed, temp->condition);
        temp = temp->next;
    }

    // printf("Fim da fila!\n\n");
}

#endif // SASHA_MONSTER_H
