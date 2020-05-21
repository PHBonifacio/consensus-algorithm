#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "sha-256.h"

typedef struct Block
{
    uint16_t index;
    clock_t timestamp;
    uint8_t last_hash[32];
    uint32_t data_len;
    char *p_data;
    struct Block *p_last_block;
    struct Block *p_next_block;
    uint64_t random;
    uint8_t block_hash[32];
} block_t;

uint8_t Block_Add(char *pdata, uint32_t data_len);
block_t* Block_Get_Index(uint32_t index);
uint8_t Block_Validate(block_t *pblock);
void Block_Print_Info(block_t *block);
void Block_Delete_All();
