#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "block.h"

Block* create_block(Block *prev, const void *data, size_t len) {
    Block *block = (Block*) malloc(sizeof(Block));

    if (block == NULL) {
        printf("[!] Failed to allocate memory for block");
        return NULL;
    }

    if (prev == NULL) {
        printf("[!] Previous block is NULL, assuming index 0");
    }

    block->index = prev ? prev->index + 1 : 0;
    block->timestamp = time(NULL);
    memset(block->data, 0, sizeof(block->hash));
    memcpy(block->data, data, (len > MAX_BLOCK_DATA_LEN) ? MAX_BLOCK_DATA_LEN : len);
    memset(block->hash, 0, sizeof(block->hash));
    block->nonce = 0; // This is set later
    block->prev = prev;
    block->next = NULL;

    return block;
}