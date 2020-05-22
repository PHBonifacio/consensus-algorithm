
#include "linkedlist.h"

#define TEST_BLOCKS     2
int main (int argc, char *argv[])
{
    for (uint8_t i = 0; i < TEST_BLOCKS; i++)
    {
        char data[10];
        sprintf(data, "Test : %2d\0", i);
        printf("%s\r\n", data);
        Block_Add(data, strlen(data));
    }
    
    for (uint8_t i = 0; i < TEST_BLOCKS; i++)
    {
        block_t *pblock = Block_Get_Index(i + 1);
        Block_Print_Info(pblock);
        Block_Validate(pblock);
    }

    for (uint8_t i = 0; i < (TEST_BLOCKS / 2); i++)
    {
        Block_Delete_Item(i * 2);
    }

    for (uint8_t i = 0; i < TEST_BLOCKS; i++)
    {
        block_t *pblock = Block_Get_Index(i + 1);
        Block_Print_Info(pblock);
        Block_Validate(pblock);
    }
    
    Block_Delete_All();
    Block_Add("New Test after delete all blocks", 33);
    Block_Print_Info(Block_Get_Index(1));

    Block_Delete_All();
    return 0;
}