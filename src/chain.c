#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Block {
    int index;
    char* data;
    char hash[65];
    time_t time;
    long long nonce;
    struct Block* next;
} Block;

typedef struct Chain {
    struct Block* head;
    struct Block* tail;
} Chain;

void init_block(Block* b) {
    if (b == NULL) {
        printf("Cannot init NULL block");
        return;
    }

    b->index = 0;
    b->data = "\nWHAT I BRING IS LIGHT\nWHAT I BRING IS A STAR\nWHAT I BRING IS\nAN ANCIENT SEA";
    memset(b->hash, 0, sizeof(b->hash));
    b->time = time(NULL);
    b->nonce = 0;
    // b->prev = NULL;
    b->next = NULL;
}

void init_chain(Chain* c) {
    if (c == NULL) {
        printf("Cannot init NULL chain");
        return;
    }

    Block *genesis = (Block*) malloc(sizeof(Block));
    init_block(genesis);

    c->head = genesis;
    c->tail = genesis;
}

void transact(Chain* c, char* data) {
    if (c == NULL) {
        printf("Cannot record transaction to null chain");
    }

    if (c->head == NULL || c->tail == NULL) {
        printf("Cannot record transaction without proper genesis block");
    }

    Block *b = (Block*) malloc(sizeof(Block));
    init_block(b);

    b->index = c->tail->index + 1;
    b->data = data;
    strncpy(b->hash, c->tail->hash, 64);
    b->hash[64] = '\0';

    c->tail->next = b;
    c->tail = b;
}

void free_chain(Chain* c) {
    if (c == NULL) {
        // Nothing to free
        return;
    }

    Block* current = c->head;
    Block* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(c);
}

int main(void) {
    Chain *c = (Chain*) malloc(sizeof(Chain));

    if (c == NULL) {
        perror("Failed to allocate memory for blockchain");
        exit(1);
    }

    init_chain(c);

    transact(c, "Hello World!");
    transact(c, "foo");
    transact(c, "bar");
    transact(c, "baz");

    Block* block = c->head;
    while (block != NULL) {
        printf("=====================================\n");
        printf("Block Loc: %p\n", block);
        printf("Block Index: %d\n", block->index);
        printf("Block Data: %s\n", block->data);
        printf("Block Hash: %s\n", block->hash);
        printf("Block Nonce: %lld\n", block->nonce);
        printf("Block Time: %ld\n", block->time);
        printf("=====================================\n\n");
        block = block->next;
    }

    free_chain(c);

    return 0;
}