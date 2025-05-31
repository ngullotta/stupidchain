#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "block.h"

Block* create_block(Block *prev, const void *data, size_t len) {
    Block *block = (Block*) malloc(sizeof(Block));

    if (block == NULL) {
        perror("[!] Failed to allocate memory for blockchain");
        return NULL;
    }

    if (prev == NULL) {
        printf("[@] Assuming block is index 0\n");
    }

    block->index = prev ? prev->index + 1 : 0;
    block->timestamp = time(NULL);
    memset(block->data, 0, MAX_BLOCK_DATA_LEN);
    memcpy(block->data, data, (len > MAX_BLOCK_DATA_LEN) ? MAX_BLOCK_DATA_LEN : len);
    memset(block->hash, 0, sizeof(block->hash));
    block->nonce = 0; // This is set later
    block->prev = prev;
    block->next = NULL;

    return block;
}

int serialize_block(const Block *block, char *buf, size_t len) {
    if (block == NULL || buf == NULL || len == 0) {
        return 0;
    }
    
    char tempbuf[9128];

    int offset = 0;

    // Metadata
    offset += snprintf(
        tempbuf + offset, 
        sizeof(tempbuf) - offset,
        "%d|%ld|%s",
        block->index, 
        block->timestamp, 
        block->data
    );

    strncpy(buf, tempbuf, offset);

    // Ensure null terminator
    buf[offset] = '\0';

    return offset;
}
