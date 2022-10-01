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
	int *ptr = write_shared_memory(shared_fd, 1);

	assert_true(ptr!=NULL);
}

void test_write_int(void ** state){
    int *ptr = write_shared_memory(shared_fd, 1);

    assert_int_equal(1, *ptr);
}

void test_write_struct(void ** state){

    Pixel_t example_pixel = {
        .valor = 80,
        .index = 0,
        .metadata_id = 1,
        .dirtyBit = false
    };

    int *ptr = write_shared_memory(shared_fd, example_pixel);

}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_no_error, setup_basic, teardown_test),
        cmocka_unit_test_setup_teardown(test_write_int, setup_basic, teardown_test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}