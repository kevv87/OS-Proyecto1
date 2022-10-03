#include "include/metadata_table.h"
#include "include/types.h"

int get_id(char* block_name, int block_size)
{
    key_t key = ftok(block_name, 0); //Devuelve una llave basada en un path (BLOCK_NAME) y un id (en este caso 0) para hacer llamadas a RAM. Siempre que el par de datos sea el mismo devuelve lo mismo
    int id = shmget(key, block_size, 0666 | IPC_CREAT);//Share memory get devuleve el id asociado a una key_t
    //printf("BLOCK_NAME: %s\n", block_name);
    //printf("Key: %d\n", key);
    //printf("id: %d\n", id);
    return id;
}

Metadata_Table_t* locate_metadata_table(char* block_name, int block_size)
{
    int id = get_id(block_name, block_size);
    Metadata_Table_t* res = (Metadata_Table_t*) shmat(id, NULL, 0); //Mapea la memoria compartida a un espacio en memoria
    return res;
}

shared_struct_metadata* initialize_metadata_table()
{
    shared_struct_metadata shs;
    shared_struct_metadata* raw_shs_ptr = &shs;
    shared_struct_metadata* shs_ptr = locate_shared_memory_block();
    memcpy(shs_ptr, raw_shs_ptr, METADATA_BLOCK_SIZE);
    return shs_ptr;
}





