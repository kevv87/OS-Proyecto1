#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmocka/include/cmocka.h"

#include "common/include/types.h"
#include "common/include/imageChunk.h"

#define SHM_1_NAME "/shm1"

int shm_size, shmid;
void * ptr_to_shm_start;
ImageChunk_t *imageChunk;

int setup_populated_list(void ** state){
    int node_quantity = 10;
    shm_size = sizeof(ImageChunk_t) + (node_quantity+1) * sizeof(Node_t);
    shmid = get_id(SHM_1_NAME, shm_size);

    ptr_to_shm_start = malloc(shm_size);
    obtain_shm_pointer(shmid, ptr_to_shm_start);

    if(ptr_to_shm_start == NULL){
        perror("obtain_shared_pointer failed");
        return -1;
    }

    // Execute
    create_image_chunk(ptr_to_shm_start, node_quantity);
    imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    return 0;
}

int teardown(void **state){
    free(ptr_to_shm_start);
    if(close_shm_ptr(shmid, ptr_to_shm_start)){
        perror("close_shm_ptr failed");
        return -1;
    }
    return 0;
}

static void create_list(void ** state){
    assert_true(imageChunk != NULL);
    assert_int_equal(imageChunk->size, 10);
}

static void initialize_mutex_semaphore(void ** state){
    int sem_value = 100;

    if(sem_init(&(imageChunk->write_semaphore), 1, 1)){
        perror("sem_init failed");
    }

    if(sem_getvalue(&(imageChunk->write_semaphore), &sem_value)){
        perror("sem_getvalue failed");
    }

    assert_int_equal(sem_value, 1);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(create_list, setup_populated_list, teardown),
        cmocka_unit_test_setup_teardown(initialize_mutex_semaphore, setup_populated_list, teardown),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}