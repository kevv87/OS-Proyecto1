#include "include/metadata_table.h"

MetadataEntry_t * initialize_metadata_table(int n_entries){
    size_t metadata_table_size = sizeof(MetadataEntry_t) * n_entries;

    int fd = obtain_shared_fd(METADATA_SHM_NAME, true, metadata_table_size);

    if(fd == -1){
        perror("failed initialize metadata");
        return NULL;
    }

    MetadataEntry_t * metadata_table =
        (MetadataEntry_t *) obtain_shared_pointer(metadata_table_size, fd);
    
    return metadata_table;
}