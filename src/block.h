#ifndef BLOCK_H
#define BLOCK_H

#include <time.h>

#define MAX_HASH_LEN 65
#define MAX_TRANSACTIONS_PER_BLOCK 10
#define MAX_BLOCK_DATA_LEN 2048

typedef struct Block {
    int index;
    time_t timestamp;
    char data[MAX_BLOCK_DATA_LEN];
    char hash[MAX_HASH_LEN];
    long long nonce;
    struct Block* prev;
    struct Block* next;
} Block;

Block* create_block(Block* previous_block, const void *data, size_t len);
#endif // BLOCK_H