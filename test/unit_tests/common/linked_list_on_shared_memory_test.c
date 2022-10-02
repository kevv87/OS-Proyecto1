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
int shm_fd;
void * ptr_to_shm_start;

// -----> Auxiliary functions

int setup_empty_list(void ** state){
    shm_size = sizeof(ImageChunk_t);
    shm_fd = obtain_shared_fd(SHM_1_NAME, true, shm_size);

    if(shm_fd == -1){
        perror("obtain_shared_fd failed");
        return -1;
    }

    ptr_to_shm_start = obtain_shared_pointer(shm_size, shm_fd);
    if(ptr_to_shm_start == NULL){
        perror("obtain_shared_pointer failed");
        return -1;
    }

    // Execute
    shm_create_image_chunk(ptr_to_shm_start, 0);
    imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    return 0;
}

int setup_populated_list(void ** state){
    int node_quantity = 10;
    shm_size = sizeof(ImageChunk_t) + node_quantity * sizeof(Node_t);
    shm_fd = obtain_shared_fd(SHM_1_NAME, true, shm_size);

    ptr_to_shm_start = obtain_shared_pointer(shm_size, shm_fd);

    // Execute
    shm_create_image_chunk(ptr_to_shm_start, node_quantity);

    return 0;
}

int teardown(void **state){
    close_shared_pointer(ptr_to_shm_start, shm_size);
    close_shared_memory(SHM_1_NAME);
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
    int shm_size = sizeof(ImageChunk_t);
    int shm_fd = obtain_shared_fd(SHM_1_NAME, true, shm_size);
    assert_int_not_equal(shm_fd, -1);

    void * ptr_to_shm_start = obtain_shared_pointer(shm_size, shm_fd);
    assert_true(ptr_to_shm_start != NULL);

    // Execute
    shm_create_image_chunk(ptr_to_shm_start, 0);
    ImageChunk_t *imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    // Assert
    assert_true(ptr_to_shm_start!=NULL);
    assert_int_equal(imageChunk->size, 0);

    // Close
    close_shared_pointer(ptr_to_shm_start, shm_size);
    close_shared_memory(SHM_1_NAME);
}

static void test_add_first_item(void **state){
    Node_t newPixel = {
        .next = NULL,
        .value = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };

    shm_append_item(ptr_to_shm_start , &newPixel);

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
    shm_append_item(ptr_to_shm_start, &firstPixel);
    shm_append_item(ptr_to_shm_start, &newPixel);

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
    shm_append_item(ptr_to_shm_start, &firstPixel);
    shm_append_item(ptr_to_shm_start, &secondPixel);
    shm_append_item(ptr_to_shm_start, &thirdPixel);
    shm_append_item(ptr_to_shm_start, &fourthPixel);

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
    int shm_size = sizeof(ImageChunk_t) + node_quantity * sizeof(Node_t);
    int shm_fd = obtain_shared_fd(SHM_1_NAME, true, shm_size);
    assert_int_not_equal(shm_fd, -1);

    void * ptr_to_shm_start = obtain_shared_pointer(shm_size, shm_fd);
    assert_true(ptr_to_shm_start != NULL);

    // Execute
    shm_create_image_chunk(ptr_to_shm_start, node_quantity);

    ImageChunk_t *imageChunk = (ImageChunk_t *) ptr_to_shm_start;

    assert_int_equal(imageChunk->size, node_quantity);
}

static void test_get_pixel_by_index(void **state){
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
    
    Node_t *nextPixel_old = get_pixel_by_index(imageChunk, 5);
    replace_nth_pixel(imageChunk, &newPixel, 4);
    Node_t *nextPixel_new = get_pixel_by_index(imageChunk, 5);
    Node_t *replacedPixel = get_pixel_by_index(imageChunk, 4);

    // Value is replaced
    assert_int_equal(replacedPixel->value, newPixel.value);
    // Index is replaced
    assert_int_equal(newPixel.index, 4);
    // Next index left untouched
    assert_int_equal(nextPixel_old->index, nextPixel_new->index);
    assert_int_equal(nextPixel_old->value, nextPixel_new->value);
}

static void test_replace_last_pixel(void **state){
    Node_t newPixel = {
        .value = 13,
        .index = -1,
        .metadata_id = 2,
        .dirtyBit = false
    };
    int list_length = imageChunk->size;

    Node_t *nextPixel_old = get_pixel_by_index(imageChunk, 0);
    replace_nth_pixel(imageChunk, &newPixel, list_length-1);
    Node_t *nextPixel_new = get_pixel_by_index(imageChunk,0);
    Node_t *replacedPixel = get_pixel_by_index(imageChunk, list_length-1);

    // Value is replaced
    assert_int_equal(replacedPixel->value, newPixel.value);
    // Index is replaced
    assert_int_equal(newPixel.index, list_length-1);
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

    assert_true(foundPixel);
    assert_int_equal(foundPixel->value, newPixel.value);
    assert_int_equal(foundPixel->index, newPixel.index);

}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(create_list),
        cmocka_unit_test_setup_teardown(test_add_first_item, setup_empty_list, teardown),
        cmocka_unit_test_setup_teardown(test_add_item_at_end, setup_empty_list, teardown),
        cmocka_unit_test_setup_teardown(test_add_several_items, setup_empty_list, teardown),
        cmocka_unit_test_setup_teardown(test_generate_fixed_length_chunk, NULL, teardown),
        // cmocka_unit_test_setup_teardown(test_get_pixel_by_index, setup_populated_list, teardown),
        // cmocka_unit_test_setup_teardown(test_replace_first_pixel, setup_populated_list, teardown),
        // cmocka_unit_test_setup_teardown(test_replace_nth_pixel, setup_populated_list, teardown),
        // cmocka_unit_test_setup_teardown(test_get_pixel_by_metadata_id, setup_populated_list, teardown),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
