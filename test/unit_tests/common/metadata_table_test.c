#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmocka/include/cmocka.h"

#include "common/include/types.h"
#include "common/include/metadata_table.h"

// ------> Tests 
static void table_exists(void **state){
    // Execute
    MetadataEntry_t * metadata_table = initialize_metadata_table(0);

    // Assert
    assert_true(metadata_table != NULL);

    // Close
    close_shared_pointer(metadata_table, sizeof(MetadataEntry_t));
    close_shared_memory(METADATA_SHM_NAME);
}

static void table_initializes_for_n_elements(void ** state){
    MetadataEntry_t * metadata_table = initialize_metadata_table(10);

    assert_true(metadata_table != NULL);

    // Close
    close_shared_pointer(metadata_table, sizeof(MetadataEntry_t));
    close_shared_memory(METADATA_SHM_NAME);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        // cmocka_unit_test_setup_teardown(test_no_error, setup_basic, teardown_test),
        cmocka_unit_test(table_exists),
        // cmocka_unit_test(table_initializes_for_n_elements),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}