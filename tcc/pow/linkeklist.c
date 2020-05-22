#include "linkedlist.h"

/**
 * @brief 
 * 
 * @param var 
 */
void cleanup_uint8(uint8_t **var)
{
    free(*var);
}
void cleanup_block(block_t **var)
{
    free(*var);
}

static block_t * pblockchain = NULL;
static uint32_t blockchain_len = 0;
static uint32_t blockchain_index = 0;
static uint32_t block_rule = 0xFF;


uint64_t  Block_Serialize(block_t* pblock, uint8_t **block_serialized)
{
    uint8_t *pblockarray;
    uint64_t blockarray_len = 0;

    if (NULL != pblock)
    {
        blockarray_len = sizeof(pblock->index) + sizeof(pblock->timestamp) +\
            sizeof(pblock->last_hash) + sizeof(pblock->data_len) + \
            pblock->data_len + sizeof(pblock->p_last_block) + \
            sizeof(pblock->random);
        pblockarray = malloc(blockarray_len);

        uint64_t index = 0;
        memset(pblockarray, 0, sizeof(pblockarray));

        for(int8_t i = sizeof(pblock->index) - 1; i >= 0; i--, index++)
        {
            pblockarray[index] = (uint8_t)(pblock->index >> (8 * i));
        }
        
        for(int8_t i = sizeof(pblock->timestamp) - 1; i >= 0; i--, index++)
        {
            pblockarray[index] = (uint8_t)(pblock->timestamp >> (8 * i));
        }
        
        memcpy(&pblockarray[index], pblock->last_hash, sizeof(pblock->last_hash));
        index += sizeof(pblock->last_hash);

        for(int8_t i = sizeof(pblock->data_len) - 1; i >= 0; i--)
        {
            pblockarray[index++] = (uint8_t)(pblock->data_len >> (8 * i));
        }

        memcpy(&pblockarray[index], pblock->p_data, pblock->data_len);
        index += pblock->data_len;
        
        uint64_t last_block_addr = (uint64_t)pblock->p_last_block;
        for(int8_t i = sizeof(last_block_addr) - 1; i >= 0; i--)
        {
            pblockarray[index++] = (uint8_t)(last_block_addr >> (8 * i));
        }

        for(int8_t i = sizeof(pblock->random) - 1; i >= 0; i--)
        {
            pblockarray[index++] = (uint8_t)(pblock->random >> (8 * i));
        }
        
        *block_serialized = pblockarray;
        return blockarray_len;
    }
    return -1;
}

uint8_t Calc_Hash(block_t *pblock)
{
    uint8_t *pblockarray __attribute__ ((__cleanup__(cleanup_uint8)));
    uint64_t blockarray_len = 0;
    uint32_t hash_end = block_rule;
    for (__uint128_t i = 0; (i < UINT64_MAX) && (0 != (hash_end & block_rule)); i++)
    {
        pblock->random = (uint64_t)i;

        blockarray_len = Block_Serialize(pblock, &pblockarray);
        calc_sha_256(pblock->block_hash, pblockarray, blockarray_len);
        hash_end = (uint32_t)(
            (pblock->block_hash[28] << 24) | (pblock->block_hash[29] << 16) | \
            (pblock->block_hash[30] << 8) |  (pblock->block_hash[31]));
    }
    printf("Index hash = %ld\r\n", pblock->random);
    return 0;
}

block_t * Block_Last()
{
    block_t * return_block = NULL;

    if (NULL == pblockchain)
    {
        pblockchain =(block_t *) malloc(sizeof(block_t));
    }
    return_block = pblockchain;

    for (uint64_t i = 0; i < blockchain_len; i++)
    {
        if (NULL != return_block->p_next_block)
        {
            return_block = return_block->p_next_block;
        }
        else
        {
            break;
        }
        
    }

    return return_block;
}

void Block_Difficulty(char op)
{
    if ('+' == op)
    {
        block_rule <<= 1;
        block_rule |= 1;
        printf ("Increasing Difficulty : %02X\r\n", block_rule);
    }
    else if ('-' == op)
    {
        block_rule >>= 1;
        printf ("Decreasing Difficulty: %02X\r\n", block_rule);
    }
    else
    {
        printf("Invalid operation\r\n");
    }
    
}
uint8_t Block_Add(char *pdata, uint32_t data_len)
{
    block_t *pblock = NULL;
    pblock = Block_Last();
    block_t *temp = (block_t *)malloc(sizeof(block_t));
    clock_t starttime, endtime;

    if (0 != blockchain_len)
    {
        temp->p_last_block = pblock;
        pblock->p_next_block = temp;
        memcpy(temp->last_hash, pblock->block_hash, sizeof(pblock->block_hash));
        pblock = temp;
    }

    pblock->index = blockchain_index++;
    blockchain_len++;
    pblock->timestamp = clock();
    pblock->data_len = data_len;
    pblock->p_data = malloc(data_len + 1);
    memset(pblock->p_data, 0, pblock->data_len);
    memcpy(pblock->p_data, pdata, data_len);

    starttime = clock();
    Calc_Hash(pblock);
    endtime = clock();
    double elapsed = endtime - starttime;
    elapsed /= CLOCKS_PER_SEC;

    printf("Hash Elapsed Time: %f\r\n", elapsed);
    if (3 < elapsed)
    {
        Block_Difficulty('-');
    }
    else if (1 > elapsed)
    {
        Block_Difficulty('+');
    }
    

}

status_t Block_Validate(block_t *pblock)
{
    uint8_t *pblockarray __attribute__ ((__cleanup__(cleanup_uint8)));
    uint64_t blockarray_len = 0;
    uint8_t hash[32] = {0};

    if (NULL != pblock)
    {
        blockarray_len = Block_Serialize(pblock, &pblockarray);
        calc_sha_256(hash, pblockarray, blockarray_len);

        if (memcmp(hash, pblock->block_hash, 32))
        {
            printf("Invalid Block!\r\n");
            return ST_INVALID_BLOCK;
        }

        printf("Valid Block!\r\n");
        return ST_OK;
    }

    return ST_NULL_POINTER;
}

block_t* Block_Get_Index(uint32_t index)
{
    block_t * return_block = NULL;

    if (index <= blockchain_len)
    {
        return_block = pblockchain;
        for (uint32_t i = 1; i < index; i++)
        {
            if  (NULL != return_block->p_next_block)
            {
                return_block = return_block->p_next_block;
            }
            else
            {
                return NULL;
            }
            
        }
    }

    return return_block;
}

void Block_Print_Info(block_t *block)
{
    if (NULL != block)
    {
        printf("%s - %ld\r\n", __FUNCTION__, block);
        printf("\t index    \t\t\t : %d \r\n", block->index);
        printf("\t timestamp\t\t\t : %d \r\n", block->timestamp);
        printf("\t last_hash\t\t\t : ");
        for (uint32_t i = 0; i < 32; i++)
        {
            printf("%02x", block->last_hash[i]);
        }
        printf("\r\n");
        printf("\t data_len\t\t\t : %d \r\n", block->data_len);
        printf("\t p_data  \t\t\t : ");
        for (uint32_t i = 0; i < block->data_len; i++)
        {
            printf("%02x", block->p_data[i]);
        }
        printf("\r\n");
        printf("\t p_last_block\t\t\t : %ld \r\n", block->p_last_block);
        printf("\t p_next_block\t\t\t : %ld \r\n", block->p_next_block);
        printf("\t random      \t\t\t : %ld \r\n", block->random);
        printf("\t block_hash\t\t\t : ");
        for (uint32_t i = 0; i < 32; i++)
        {
            printf("%02x", block->block_hash[i]);
        }
        printf("\r\n");
    }
}

void Block_Delete_All()
{
    block_t *ptemp;
    for (uint32_t i = 0; i < blockchain_len; i++)
    {
        ptemp = pblockchain->p_next_block;
        free(pblockchain);
        pblockchain = ptemp;
    }
    
    blockchain_len = 0;
}

/**
 * @brief Deletes an item based on its index.
 * 
 * @param index Item index.
 * @return status_t Operation result.
 */
status_t Block_Delete_Item(uint32_t index)
{
    block_t * pblock __attribute__ ((__cleanup__(cleanup_block)))= NULL;
    if (NULL != (pblock = Block_Get_Index(index)))
    {
        if (NULL != pblock->p_last_block)
        {
            pblock->p_last_block->p_next_block = pblock->p_next_block;
        }

        if (NULL != pblock->p_next_block)
        {
            pblock->p_next_block->p_last_block = pblock->p_last_block;
        }    
        
        blockchain_len--;
        
        if (pblockchain == pblock)
        {
            pblockchain = pblock->p_next_block;
            free(pblock);
        }
        return ST_OK;
    }
    return ST_NOT_FOUND;
}