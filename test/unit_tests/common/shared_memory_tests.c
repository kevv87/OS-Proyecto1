#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmocka/include/cmocka.h"

#include "common/include/shared_memory.h"
#include "common/include/types.h"

int shared_fd;

// ****************************************************************
// Setup

int setup_basic( void ** state){
    shared_fd = obtain_shared_fd("test", true, 4096);
    if (shared_fd == -1)
        return -1;
    return 0;
}

int teardown_test(void ** state){
    if(close_shared_memory("test") == -1)
        return -1;

    return 0;
}

// ****************************************************************
// Tests

void test_no_error(void ** state){
    int toWrite = 1;
    Pixel_t *shm_ptr = obtain_shared_pointer(sizeof(int), shared_fd);
    assert_true(shm_ptr != NULL);

	int *ptr = write_shared_memory(shm_ptr, &toWrite, sizeof(int));

	assert_true(ptr!=NULL);
}

void test_write_int(void ** state){
    int toWrite = 1;
    Pixel_t *shm_ptr = obtain_shared_pointer(sizeof(int), shared_fd);

	int *ptr = (int *) write_shared_memory(shm_ptr, &toWrite, sizeof(int));

    assert_int_equal(1, *ptr);
}

void test_write_struct(void ** state){
    Pixel_t example_pixel = {
        .value = 80,
        .index = 0,
        .metadata_id = 1,
        .dirtyBit = false
    };

    Pixel_t *shm_ptr = obtain_shared_pointer(sizeof(Pixel_t), shared_fd);

    Pixel_t *ptr = (Pixel_t *) write_shared_memory(shm_ptr, &example_pixel, sizeof(example_pixel));

    assert_int_equal(example_pixel.value, ptr->value);
    assert_int_equal(example_pixel.dirtyBit, ptr->dirtyBit);
}

void test_read_int(void ** state){

    int valueToWrite = 1;

    int *shm_ptr = obtain_shared_pointer(sizeof(int), shared_fd);

    write_shared_memory(shm_ptr, &valueToWrite, sizeof(int));
    int *read_ptr = malloc(sizeof(int));
    read_shared_memory(read_ptr, shm_ptr, sizeof(int));

    assert_true(read_ptr!= NULL);
    assert_int_equal(*read_ptr, valueToWrite);

    free(read_ptr);
    close_shared_pointer(shm_ptr, sizeof(int));
}

void test_read_struct(void ** state){
    Pixel_t example_pixel = {
        .value = 80,
        .index = 0,
        .metadata_id = 1,
        .dirtyBit = false
    };

    Pixel_t *shm_ptr = obtain_shared_pointer(sizeof(Pixel_t), shared_fd);

    Pixel_t *ptr = (Pixel_t *) write_shared_memory(shm_ptr, &example_pixel, sizeof(example_pixel));
    
    Pixel_t *read_ptr = malloc(sizeof(Pixel_t));
    read_shared_memory(read_ptr, shm_ptr, sizeof(Pixel_t));

    assert_true(read_ptr!= NULL);
    assert_int_equal(read_ptr->value, example_pixel.value);
    assert_int_equal(read_ptr->dirtyBit, example_pixel.dirtyBit);

    free(read_ptr);
    close_shared_pointer(shm_ptr, sizeof(int));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_no_error, setup_basic, teardown_test),
        cmocka_unit_test_setup_teardown(test_write_int, setup_basic, teardown_test),
        cmocka_unit_test_setup_teardown(test_write_struct, setup_basic, teardown_test),
        cmocka_unit_test_setup_teardown(test_read_int, setup_basic, teardown_test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}