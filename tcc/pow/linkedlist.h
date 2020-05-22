#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "sha-256.h"

typedef enum status
{
    ST_OK               =  0,
    ST_GENERAL_ERROR    = -1,
    ST_NOT_FOUND        = -2,  
    ST_INVALID_BLOCK    = -3,
    ST_NULL_POINTER     = -4,
} status_t;

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
status_t Block_Validate(block_t *pblock);
void Block_Print_Info(block_t *block);
void Block_Delete_All();
status_t Block_Delete_Item(uint32_t index);
