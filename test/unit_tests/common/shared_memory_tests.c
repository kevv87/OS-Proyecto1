#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmocka/include/cmocka.h"

#include "common/include/shared_memory.h"
#include "common/include/types.h"

#define SHM_NAME "/shm1"

int shared_fd;

// ****************************************************************
// Setup

// int setup_basic( void ** state){
//     shared_fd = obtain_shared_fd("test", true, 4096);
//     if (shared_fd == -1)
//         return -1;
//     return 0;
// }

int teardown_test(void ** state){
    if(close_shared_memory("test") == -1)
        return -1;

    return 0;
}

// ****************************************************************
// Tests

void test_no_error(void ** state){
    int toWrite = 1;

    Node_t *shm_ptr = obtain_shm_pointer(SHM_NAME, sizeof(int));

    assert_true(shm_ptr != NULL);

	int *ptr = write_shared_memory(shm_ptr, &toWrite, sizeof(int));

	assert_true(ptr!=NULL);
    close_shm_ptr(shm_ptr);
}

void test_write_int(void ** state){
    int toWrite = 1;
    Node_t *shm_ptr = obtain_shm_pointer(SHM_NAME, sizeof(int));

	int *ptr = (int *) write_shared_memory(shm_ptr, &toWrite, sizeof(int));

    assert_int_equal(1, *ptr);
    close_shm_ptr(shm_ptr);
}

void test_write_struct(void ** state){
    Node_t example_pixel = {
        .value = 80,
        .index = 0,
        .metadata_id = 1,
        .dirtyBit = false
    };

    Node_t *shm_ptr = obtain_shm_pointer(SHM_NAME, sizeof(Node_t));

    Node_t *ptr = (Node_t *) write_shared_memory(shm_ptr, &example_pixel, sizeof(example_pixel));

    assert_int_equal(example_pixel.value, ptr->value);
    assert_int_equal(example_pixel.dirtyBit, ptr->dirtyBit);
    close_shm_ptr(shm_ptr);
}

void test_read_int(void ** state){

    int valueToWrite = 1;

    int *shm_ptr = obtain_shm_pointer(SHM_NAME, sizeof(int));

    write_shared_memory(shm_ptr, &valueToWrite, sizeof(int));
    int *read_ptr = malloc(sizeof(int));
    read_shared_memory(read_ptr, shm_ptr, sizeof(int));

    assert_true(read_ptr!= NULL);
    assert_int_equal(*read_ptr, valueToWrite);

    free(read_ptr);
    close_shm_ptr(shm_ptr);
}

void test_read_struct(void ** state){
    Node_t example_pixel = {
        .value = 80,
        .index = 0,
        .metadata_id = 1,
        .dirtyBit = false
    };

    Node_t *shm_ptr = obtain_shm_pointer(SHM_NAME, sizeof(Node_t));

    Node_t *ptr = (Node_t *) write_shared_memory(shm_ptr, &example_pixel, sizeof(example_pixel));
    
    Node_t *read_ptr = malloc(sizeof(Node_t));
    read_shared_memory(read_ptr, shm_ptr, sizeof(Node_t));

    assert_true(read_ptr!= NULL);
    assert_int_equal(read_ptr->value, example_pixel.value);
    assert_int_equal(read_ptr->dirtyBit, example_pixel.dirtyBit);

    free(read_ptr);
    close_shm_ptr(shm_ptr);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_no_error),
        cmocka_unit_test(test_write_int),
        cmocka_unit_test(test_write_struct),
        cmocka_unit_test(test_read_int),
        cmocka_unit_test(test_read_struct),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}