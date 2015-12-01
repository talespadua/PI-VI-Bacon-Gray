#ifndef SASHA_QUEUE_H
#define SASHA_QUEUE_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Node Node;
typedef struct Queue Queue;

struct Node {
    int x, y, w; // x, y, weight
    Node *next;
};

struct Queue {
    int size;
    Node *head;
    Node *tail;
};

Queue* queue_create();
void queue_clear(Queue *q);
void queue_free(Queue *q);
void queue_enq(Queue *q, int x, int y, int w);
void queue_enqNode(Queue *q, Node *n);
Node* queue_deq(Queue *q);
int queue_isEmpty(Queue *q);
Node* queue_peek(Queue *q);
void queue_printNode(Node *n);
void queue_print(Queue *q);

Queue* queue_create() {
    Queue *q = (Queue*) malloc(sizeof(Queue));
    q->size = 0;
    q->head = q->tail = NULL;

    return q;
}

void queue_clear(Queue *q) {
    while (q->head != NULL) {
        Node *temp = q->head;
        q->head = temp->next;

        // printf("Removing: ");
        // queue_printNode(temp);
        free(temp);
    }

    q->size = 0;
    q->tail = NULL;
}

void queue_free(Queue *q) {
    queue_clear(q);
    free (q);
    // printf("Done!\n");
}

void queue_enq(Queue *q, int x, int y, int w) {
    Node *n = (Node*) malloc(sizeof(Node));
    n->x = x;
    n->y = y;
    n->w = w;

    queue_enqNode(q, n);
}

void queue_enqNode(Queue *q, Node *n) {
    n->next = NULL;

    if (queue_isEmpty(q)) {
        q->head = q->tail = n;
    } else {
        q->tail->next = n;
        q->tail = n;
    }

    q->size++;
}

Node* queue_deq(Queue *q) {
    if (queue_isEmpty(q)) {
        printf("Fila vazia!\n");
        return NULL;
    }

    Node *temp = q->head;
    q->head = temp->next;
    q->size--;

    if (queue_isEmpty(q)) {
        q->tail = q->head;
    }

    return temp;
}

int queue_isEmpty(Queue *q) {
    return (q->size == 0);
}

Node* queue_peek(Queue *q) {
    return q->head;
}

void queue_printNode(Node *n) {
    printf("x: %d, y: %d, w: %d \n", n->x, n->y, n->w);
}

void queue_print(Queue *q) {
    Node *temp = q->head;

    while(temp != NULL) {
        queue_printNode(temp);
        temp = temp->next;
    }

    // printf("Fim da fila!\n\n");
}

#endif // SASHA_QUEUE_H
