#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "chain.h"

static const char* GENESIS_DATA = "\nWHAT I BRING IS LIGHT\nWHAT I BRING IS A STAR\nWHAT I BRING IS\nAN ANCIENT SEA\0";

Chain* create_chain() {
    Chain *chain = (Chain*) malloc(sizeof(Chain));

    if (chain == NULL) {
        perror("Failed to allocate memory for blockchain");
        return NULL;
    }

    chain->nblocks = 0;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        chain->blocks[i] = NULL;
    }

    Block* genesis = create_block(NULL, GENESIS_DATA, sizeof(GENESIS_DATA));
    if (genesis == NULL) {
        free(chain);
        return NULL;
    }

    if (add_block(chain, genesis) != 0) {
        fprintf(stderr, "Error: Failed to add block to chain\n");
        free(genesis);
        free(chain);
        return NULL;
    }
}

int add_block(Chain* chain, Block* block) {
    if (chain == NULL) {
        fprintf(stderr, "Error: Chain is null\n");
        return -1;
    }

    if (block == NULL) {
        fprintf(stderr, "Error: Block is null\n");
        return -1;
    }

    if (chain->nblocks >= MAX_BLOCKS) {
        fprintf(stderr, "Error: Blockchain is full, cannot add more blocks.\n");
        return -1;
    }

    chain->blocks[chain->nblocks++] = block;
    return 0;
}

void free_chain(Chain* chain) {
    if (chain == NULL) {
        return; // Nothing to free
    }

    Block* current = chain->head;
    Block* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    free(chain);
}

// int main(void) {
//     Chain *c = (Chain*) malloc(sizeof(Chain));

//     if (c == NULL) {
//         perror("Failed to allocate memory for blockchain");
//         exit(1);
//     }

//     init_chain(c);

//     transact(c, "Hello World!");
//     transact(c, "foo");
//     transact(c, "bar");
//     transact(c, "baz");

//     Block* block = c->head;
//     while (block != NULL) {
//         printf("=====================================\n");
//         printf("Block Loc: %p\n", block);
//         printf("Block Index: %d\n", block->index);
//         printf("Block Data: %s\n", block->data);
//         printf("Block Hash: %s\n", block->hash);
//         printf("Block Nonce: %lld\n", block->nonce);
//         printf("Block Time: %ld\n", block->time);
//         printf("=====================================\n\n");
//         block = block->next;
//     }

//     free_chain(c);

//     return 0;
// }