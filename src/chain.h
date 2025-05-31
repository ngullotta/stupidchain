#ifndef CHAIN_H
#define CHAIN_H

#include "block.h"

#define MAX_BLOCKS 0x100000000
#define DIFFICULTY 2

typedef struct Chain {
    int nblocks;
    Block* blocks[MAX_BLOCKS];
    Block* head;
    Block* tail;
} Chain;

Chain* create_chain(void);

int add_block(Chain *chain, Block *block);
void free_chain(Chain *chain);

#endif // CHAIN_H
