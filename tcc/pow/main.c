
#include "linkedlist.h"

int main (int argc, char *argv[])
{
    Block_Add("Teste1", 7);
    Block_Add("Teste2", 7);
    Block_Add("Teste3", 7);
    Block_Add("Teste4", 7);
    Block_Add("Teste5", 7);
    Block_Add("Teste6", 7);
    
    Block_Print_Info(Block_Get_Index(1));
    Block_Print_Info(Block_Get_Index(2));
    Block_Print_Info(Block_Get_Index(3));
    Block_Print_Info(Block_Get_Index(4));
    Block_Print_Info(Block_Get_Index(5));
    Block_Print_Info(Block_Get_Index(6));
    
    Block_Validate(Block_Get_Index(1));
    Block_Validate(Block_Get_Index(2));
    Block_Validate(Block_Get_Index(3));
    Block_Validate(Block_Get_Index(4));
    Block_Validate(Block_Get_Index(5));
    Block_Validate(Block_Get_Index(6));
    
    Block_Delete_All();
    Block_Add("Teste6", 7);


    return 0;
}