#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmocka/include/cmocka.h"

#include "common/include/imageChunk.h"
#include "common/include/shared_memory.h"

#define SHM_1_NAME "/test1"


ImageChunk_t *imageChunk;
int shm_size;
int shmid;
void * ptr_to_shm_start;

// -----> Auxiliary functions

int setup_empty_list(void ** state){
    shm_size = sizeof(ImageChunk_t)+sizeof(Node_t);

    shmid = get_id(SHM_1_NAME, shm_size);
    ptr_to_shm_start = malloc(shm_size);

    obtain_shm_pointer(shmid, ptr_to_shm_start);
    if(ptr_to_shm_start == NULL){
        perror("obtain_shared_pointer failed");
        return -1;
    }

    create_image_chunk(ptr_to_shm_start, 0);
    imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    return 0;
}

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

Node_t * get_shm_ptr_by_node_index(void *shm_ptr_to_start, int index){
    return ptr_to_shm_start     + 
        sizeof(ImageChunk_t)    +
        index * sizeof(Node_t);
}

// ------> Tests 
static void create_list(void **state){
    // Setup
    size_t shm_size = sizeof(ImageChunk_t)+sizeof(Node_t);
    int shmid = get_id(SHM_1_NAME, shm_size);

    void * ptr_to_shm_start = malloc(shm_size);
    
    obtain_shm_pointer(shmid, ptr_to_shm_start);
    assert_true(ptr_to_shm_start != NULL);

    // Execute
    create_image_chunk(ptr_to_shm_start, 0);
    ImageChunk_t *imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    // Assert
    assert_true(ptr_to_shm_start!=NULL);
    assert_int_equal(imageChunk->size, 0);

    // Close
    free(ptr_to_shm_start);
    close_shm_ptr(shmid, ptr_to_shm_start);
}

static void test_add_first_item(void **state){
    Node_t newPixel = {
        .next = NULL,
        .value = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };

    append_item(ptr_to_shm_start , &newPixel);

    // Normal assertions
    assert_true(imageChunk->size == 1);
    assert_true(imageChunk->head->value == newPixel.value);
    assert_true(imageChunk->head == imageChunk->tail);
    assert_true(newPixel.index == 0);

    // Shared memory assertions
    Node_t * shm_node = 
        ptr_to_shm_start + 
        sizeof(ImageChunk_t);
    
    assert_true(shm_node->value == newPixel.value);
    assert_true(shm_node->dirtyBit == newPixel.dirtyBit);
}

static void test_add_item_at_end(void **state){
    Node_t firstPixel = {
        .next = NULL,
        .value = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };
    
    Node_t newPixel = {
        .next = NULL,
        .value = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };

    // Execution 
    append_item(ptr_to_shm_start, &firstPixel);
    append_item(ptr_to_shm_start, &newPixel);

    // Shared memory addresses
    Node_t * firstPixel_ptr = 
        get_shm_ptr_by_node_index(ptr_to_shm_start , 0);
    
    Node_t * newPixel_ptr =
        get_shm_ptr_by_node_index(ptr_to_shm_start , 1);
    
    imageChunk = (ImageChunk_t *) ptr_to_shm_start;
    
    // List validations
    assert_true(imageChunk->size == 2);
    assert_true(imageChunk->head == firstPixel_ptr);
    assert_true(imageChunk->tail == newPixel_ptr);
    
    // Individual pixels validations
    assert_true(firstPixel.index == 0);
    assert_true(newPixel.index == 1);

    // Node validations
    assert_true(imageChunk->head->next == newPixel_ptr);
        // Circularity validations
    assert_true(imageChunk->tail->next == imageChunk->head);

    // Shared memory assertions
    assert_true(newPixel_ptr->value == newPixel.value);
    assert_true(newPixel_ptr->dirtyBit == newPixel.dirtyBit);

}

static void test_add_several_items(void **state){

    Node_t firstPixel = {
        .next = NULL,
        .value = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };
    
    Node_t secondPixel = {
        .next = NULL,
        .value = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };

    Node_t thirdPixel = {
        .next = NULL,
        .value = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };
    
    Node_t fourthPixel = {
        .next = NULL,
        .value = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };

    // Execution 
    append_item(ptr_to_shm_start, &firstPixel);
    append_item(ptr_to_shm_start, &secondPixel);
    append_item(ptr_to_shm_start, &thirdPixel);
    append_item(ptr_to_shm_start, &fourthPixel);

    // Shared memory addresses
    imageChunk = (ImageChunk_t *) ptr_to_shm_start;
    Node_t * firstPixel_ptr = 
        get_shm_ptr_by_node_index(ptr_to_shm_start , 0);
    Node_t * secondPixel_ptr = 
        get_shm_ptr_by_node_index(ptr_to_shm_start , 1);
    Node_t * thirdPixel_ptr = 
        get_shm_ptr_by_node_index(ptr_to_shm_start , 2);
    Node_t * fourthPixel_ptr = 
        get_shm_ptr_by_node_index(ptr_to_shm_start , 3);
    
    // List validations
    assert_true(imageChunk->size == 4);
    assert_true(imageChunk->head == firstPixel_ptr);
    assert_true(imageChunk->tail == fourthPixel_ptr);
    
    // Individual pixels validations
    assert_true(firstPixel.index == 0);
    assert_true(secondPixel.index == 1);
    assert_true(thirdPixel.index == 2);
    assert_true(fourthPixel.index == 3);

    // Node validations
    assert_true(imageChunk->head->next == secondPixel_ptr);
        // Circularity validations
    assert_true(imageChunk->tail->next == imageChunk->head);

}

static void test_generate_fixed_length_chunk(void **state){
    // Setup
    int node_quantity = 10;
    int shm_size = sizeof(ImageChunk_t) + (1+node_quantity) * sizeof(Node_t);
    int shmid = get_id(SHM_1_NAME, shm_size);

    void * ptr_to_shm_start = malloc(shm_size);
    obtain_shm_pointer(shmid, ptr_to_shm_start);

    if(ptr_to_shm_start == NULL){
        perror("obtain_shared_pointer failed");
        return;
    }

    // Execute
    create_image_chunk(ptr_to_shm_start, node_quantity);

    ImageChunk_t *imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    assert_int_equal(imageChunk->size, node_quantity);

    close_shm_ptr(shmid, ptr_to_shm_start);
    free(ptr_to_shm_start);
}

static void test_get_pixel_by_index(void **state){

    ImageChunk_t *imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    Node_t *first_pixel = get_pixel_by_index(imageChunk, 0);
    Node_t *nth_pixel = get_pixel_by_index(imageChunk, 3);
    Node_t *last_pixel = get_pixel_by_index(imageChunk, imageChunk->size-1);

    // assert_int_equal(first_pixel->index, 0);
    assert_int_equal(last_pixel->index, imageChunk->size-1);
    assert_int_equal(nth_pixel->index, 3);
}

static void test_replace_first_pixel(void **state){
    Node_t newPixel = {
        .next = NULL,
        .value = 13,
        .index = -1,
        .metadata_id = 2,
        .dirtyBit = false
    };

    ImageChunk_t *imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    replace_nth_pixel(imageChunk, &newPixel, 0);

    assert_int_equal(imageChunk->head->value, newPixel.value);
}

static void test_replace_nth_pixel(void **state){
    Node_t newPixel = {
        .value = 13,
        .index = -1,
        .metadata_id = 2,
        .dirtyBit = false
    };

    imageChunk = (ImageChunk_t *) ptr_to_shm_start;
    
    Node_t *nextPixel_old = get_pixel_by_index(imageChunk, 5);
    replace_nth_pixel(imageChunk, &newPixel, 4);
    Node_t *nextPixel_new = get_pixel_by_index(imageChunk, 5);
    Node_t *replacedPixel = get_pixel_by_index(imageChunk, 4);

    // Value is replaced
    assert_int_equal(replacedPixel->value, newPixel.value);
    // Index is replaced
    Node_t * newPixel_ptr = 
        get_shm_ptr_by_node_index(ptr_to_shm_start, 4);
    assert_int_equal(newPixel_ptr->index, 4);
    // Next index left untouched
    assert_int_equal(nextPixel_old->index, nextPixel_new->index);
    assert_int_equal(nextPixel_old->value, nextPixel_new->value);
}

static void test_get_pixel_by_metadata_id(void **state){
    Node_t newPixel = {
        .value = 13,
        .index = -1,
        .metadata_id = 20,
        .dirtyBit = false
    };
    
    replace_nth_pixel(imageChunk, &newPixel, 5);

    Node_t *foundPixel = get_pixel_by_metadata_id(imageChunk, 20);

    Node_t *newPixel_ptr = get_shm_ptr_by_node_index(ptr_to_shm_start, 5);

    assert_true(foundPixel);
    assert_int_equal(foundPixel->value, newPixel_ptr->value);
    assert_int_equal(foundPixel->index, newPixel_ptr->index);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(create_list),
        cmocka_unit_test_setup_teardown(test_add_first_item, setup_empty_list, teardown),
        cmocka_unit_test_setup_teardown(test_add_item_at_end, setup_empty_list, teardown),
        cmocka_unit_test_setup_teardown(test_add_several_items, setup_empty_list, teardown),
        cmocka_unit_test(test_generate_fixed_length_chunk),
        cmocka_unit_test_setup_teardown(test_get_pixel_by_index, setup_populated_list, teardown),
        cmocka_unit_test_setup_teardown(test_replace_first_pixel, setup_populated_list, teardown),
        cmocka_unit_test_setup_teardown(test_replace_nth_pixel, setup_populated_list, teardown),
        cmocka_unit_test_setup_teardown(test_get_pixel_by_metadata_id, setup_populated_list, teardown),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
