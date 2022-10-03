#include "include/metadata_table.h"
#include "include/types.h"

static int get_id()
{
    key_t key = ftok(METADATA_BLOCK_NAME, 0);
    return shmget(key, METADATA_BLOCK_SIZE, 0664 | IPC_CREAT);
}
shared_struct_metadata* locate_shared_memory_block()
{
    int id = get_id();
    shared_struct_metadata * res = (shared_struct_metadata*) shmat(id, NULL, 0);
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
