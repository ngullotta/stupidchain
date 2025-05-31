#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "chain.h"

static const char* GENESIS_DATA = "WHAT I BRING IS LIGHT\nWHAT I BRING IS A STAR\nWHAT I BRING IS\nAN ANCIENT SEA";

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

    Block* genesis = create_block(NULL, GENESIS_DATA, strlen(GENESIS_DATA));
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

    return chain;
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
        fprintf(stderr, "Error: Chain is full, cannot add more blocks\n");
        return -1;
    }

    chain->blocks[chain->nblocks++] = block;
    return 0;
}

void free_chain(Chain* chain) {
    if (chain == NULL) {
        return; // Nothing to free
    }

    // Free all the blocks
    for (int i = 0; i < chain->nblocks; i++) {
        if (chain->blocks[i] != NULL) {
            free(chain->blocks[i]);
        }
    }

    // Free the chain
    free(chain);
}
