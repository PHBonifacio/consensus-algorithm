#include "linkedlist.h"

void cleanup_uint8(uint8_t **var)
{
    //printf("Cleaning var\r\n");
    free(*var);
}
void cleanup_block(block_t **var)
{
    //printf("Cleaning var\r\n");
    free(*var);
}

static block_t * pblockchain = NULL;
static uint32_t blockchain_len = 0;
static uint32_t block_rule = 0xFF;


uint64_t  Block_Serialize(block_t* pblock, uint8_t **block_serialized)
{
    uint8_t *pblockarray;
    uint64_t blockarray_len = 0;
    /* printf("Sizes:\r\n");
    printf("\t index  : %ld\r\n", sizeof(pblock->index));
    printf("\t timestamp  : %ld\r\n", sizeof(pblock->timestamp));
    printf("\t last_hash  : %ld\r\n", sizeof(pblock->last_hash));
    printf("\t data_len   : %ld\r\n", sizeof(pblock->data_len));
    printf("\t data_len   : %d\r\n", pblock->data_len);
    printf("\t random     : %ld\r\n", sizeof(pblock->random));*/
    //printf("\t p_last_block     : %ld\r\n", sizeof(pblock->p_last_block)); 

    blockarray_len = sizeof(pblock->index) + sizeof(pblock->timestamp) + \
        sizeof(pblock->last_hash) + sizeof(pblock->data_len) + pblock->data_len\
        + sizeof(pblock->p_last_block) + sizeof(pblock->random);
    pblockarray = malloc(blockarray_len);
    //printf("\t pblockarray: %ld\r\n", blockarray_len);
    
    if (NULL != pblockarray)
    {
        uint64_t index = 0;
        memset(pblockarray, 0, sizeof(pblockarray));

        for(int8_t i = sizeof(pblock->index) - 1; i >= 0; i--, index++)
        {
            pblockarray[index] = (uint8_t)(pblock->index >> (8 * i));
        }
        
        //printf("Timestamp: %ld :", pblock->timestamp);
        uint8_t value;
        for(int8_t i = sizeof(pblock->timestamp) - 1; i >= 0; i--, index++)
        {
            value = (uint8_t)(pblock->timestamp >> (8 * i));
            pblockarray[index] = (uint8_t)(pblock->timestamp >> (8 * i));
            //printf("%02X", pblockarray[index]);
        }
        //printf("\r\n");
        
        memcpy(&pblockarray[index], pblock->last_hash, sizeof(pblock->last_hash));
        index += sizeof(pblock->last_hash);

        for(int8_t i = sizeof(pblock->data_len) - 1; i >= 0; i--)
        {
            value = (uint8_t)(pblock->data_len >> (8 * i));
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

        //printf("%d - pblockarray: %ld :", __LINE__, blockarray_len);
        for(uint32_t i = 0; i < blockarray_len; i++)
        {
            //printf("%02x", pblockarray[i]);
        }
        //printf("\r\n");
        //printf("pblockarray : %ld\r\n", pblockarray);
        
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
    for (__uint128_t i = 0; (i < UINT32_MAX) && (0 != (hash_end & block_rule)); i++)
    {
        pblock->random = (uint64_t)i;

        blockarray_len = Block_Serialize(pblock, &pblockarray);
        calc_sha_256(pblock->block_hash, pblockarray, blockarray_len);
        hash_end = (uint32_t)(
            (pblock->block_hash[28] << 24) | (pblock->block_hash[29] << 16) | \
            (pblock->block_hash[30] << 8) |  (pblock->block_hash[31]));
    }
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

uint8_t Block_Add(char *pdata, uint32_t data_len)
{
    block_t *pblock = NULL;
    pblock = Block_Last();
    block_t *temp = (block_t *)malloc(sizeof(block_t));

    if (0 != blockchain_len)
    {
        temp->p_last_block = pblock;
        pblock->p_next_block = temp;
        memcpy(temp->last_hash, pblock->block_hash, sizeof(pblock->block_hash));
        pblock = temp;
    }

    pblock->index = blockchain_len++;
    pblock->timestamp = clock();
    pblock->data_len = data_len;
    pblock->p_data = malloc(data_len + 1);
    memset(pblock->p_data, 0, pblock->data_len);
    memcpy(pblock->p_data, pdata, data_len);

    Calc_Hash(pblock);
}

uint8_t Block_Validate(block_t *pblock)
{
    uint8_t *pblockarray __attribute__ ((__cleanup__(cleanup_uint8)));
    uint64_t blockarray_len = 0;
    uint8_t hash[32] = {0};

    blockarray_len = Block_Serialize(pblock, &pblockarray);
    calc_sha_256(hash, pblockarray, blockarray_len);

    if (memcmp(hash, pblock->block_hash, 32))
    {
        printf("Invalid Block!\r\n");    
        return -1;
    }

    printf("Valid Block!\r\n");    
    return 0;
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
        printf("\t random      \t\t\t : %d \r\n", block->random);
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
    block_t *ptemp, *ptemp2;

    ptemp = pblockchain;

    for (uint32_t i = 0; i < blockchain_len; i++)
    {
        ptemp2 = ptemp->p_next_block;
        free(ptemp);
        ptemp = ptemp2;
    }
    
    ptemp = (block_t *) malloc(sizeof(block_t));
    blockchain_len = 0;
}

uint8_t Block_Delete_Item(uint32_t index)
{
    
}