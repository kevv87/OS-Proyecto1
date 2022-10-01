#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmocka/include/cmocka.h"

#include "common/include/imageChunk.h"

ImageChunk_t *imageChunk;

// -----> Auxiliary functions

int setup_empty_list(void ** state){
    imageChunk = malloc(sizeof(ImageChunk_t));

    ImageChunk_t imageChunk_obj = create_image_chunk(0);
    *imageChunk = imageChunk_obj;

    return 0;
}

int setup_populated_list(void ** state){
    imageChunk = malloc(sizeof(ImageChunk_t));

    ImageChunk_t imageChunk_obj = create_image_chunk(10);
    *imageChunk = imageChunk_obj;

    return 0;
}

int teardown(void **state){
    free(imageChunk);
    return 0;
}

// ------> Tests 
static void create_list(void **state){
    ImageChunk_t imageChunk = create_image_chunk(0);

    assert_non_null(&imageChunk);
    assert_true(imageChunk.size==0);
}

static void test_add_first_item(void **state){
    Pixel_t newPixel = {
        .valor = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };

    append_item(imageChunk, &newPixel);
    assert_true(imageChunk->size == 1);
    assert_true(imageChunk->head->pixel->valor == newPixel.valor);
    assert_true(imageChunk->head == imageChunk->tail);
    assert_true(newPixel.index == 0);
}

static void test_add_item_at_end(void **state){
    Pixel_t firstPixel = {
        .valor = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };
    
    Pixel_t newPixel = {
        .valor = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };

    // Execution 
    append_item(imageChunk, &firstPixel);
    append_item(imageChunk, &newPixel);
    
    // List validations
    assert_true(imageChunk->size == 2);
    assert_true(imageChunk->head->pixel == &firstPixel);
    assert_true(imageChunk->tail->pixel == &newPixel);
    
    // Individual pixels validations
    assert_true(firstPixel.index == 0);
    assert_true(newPixel.index == 1);

    // Node validations
    assert_true(imageChunk->head->next->pixel == &newPixel);
        // Circularity validations
    assert_true(imageChunk->tail->next == imageChunk->head);
}

static void test_add_several_items(void **state){

    Pixel_t firstPixel = {
        .valor = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };
    
    Pixel_t secondPixel = {
        .valor = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };
    Pixel_t thirdPixel = {
        .valor = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };
    
    Pixel_t fourthPixel = {
        .valor = 80,
        .index = -1,
        .metadata_id = 0,
        .dirtyBit = false
    };

    // Execution 
    append_item(imageChunk, &firstPixel);
    append_item(imageChunk, &secondPixel);
    append_item(imageChunk, &thirdPixel);
    append_item(imageChunk, &fourthPixel);
    
    // List validations
    assert_true(imageChunk->size == 4);
    assert_true(imageChunk->head->pixel == &firstPixel);
    assert_true(imageChunk->tail->pixel == &fourthPixel);
    
    // Individual pixels validations
    assert_true(firstPixel.index == 0);
    assert_true(secondPixel.index == 1);
    assert_true(thirdPixel.index == 2);
    assert_true(fourthPixel.index == 3);

    // Node validations
    assert_true(imageChunk->head->next->pixel == &secondPixel);
        // Circularity validations
    assert_true(imageChunk->tail->next == imageChunk->head);

}

static void test_generate_fixed_length_chunk(void **state){
    int node_quantity = 10;
    ImageChunk_t imageChunk = create_image_chunk(node_quantity);

    assert_int_equal(imageChunk.size, node_quantity);

}

static void test_get_pixel_by_index(void **state){
    Pixel_t *first_pixel = get_pixel_by_index(imageChunk, 0);
    Pixel_t *nth_pixel = get_pixel_by_index(imageChunk, 3);
    Pixel_t *last_pixel = get_pixel_by_index(imageChunk, imageChunk->size-1);

    // assert_int_equal(first_pixel->index, 0);
    assert_int_equal(last_pixel->index, imageChunk->size-1);
    assert_int_equal(nth_pixel->index, 3);
}

static void test_replace_first_pixel(void **state){
    Pixel_t newPixel = {
        .valor = 13,
        .index = -1,
        .metadata_id = 2,
        .dirtyBit = false
    };

    replace_nth_pixel(imageChunk, &newPixel, 0);

    assert_int_equal(imageChunk->head->pixel->valor, newPixel.valor);
}

static void test_replace_nth_pixel(void **state){
    Pixel_t newPixel = {
        .valor = 13,
        .index = -1,
        .metadata_id = 2,
        .dirtyBit = false
    };
    
    Pixel_t *nextPixel_old = get_pixel_by_index(imageChunk, 5);
    replace_nth_pixel(imageChunk, &newPixel, 4);
    Pixel_t *nextPixel_new = get_pixel_by_index(imageChunk,5);
    Pixel_t *replacedPixel = get_pixel_by_index(imageChunk, 4);

    // Value is replaced
    assert_int_equal(replacedPixel->valor, newPixel.valor);
    // Index is replaced
    assert_int_equal(newPixel.index, 4);
    // Next index left untouched
    assert_int_equal(nextPixel_old->index, nextPixel_new->index);
    assert_int_equal(nextPixel_old->valor, nextPixel_new->valor);
}

static void test_replace_last_pixel(void **state){
    Pixel_t newPixel = {
        .valor = 13,
        .index = -1,
        .metadata_id = 2,
        .dirtyBit = false
    };
    int list_length = imageChunk->size;

    Pixel_t *nextPixel_old = get_pixel_by_index(imageChunk, 0);
    replace_nth_pixel(imageChunk, &newPixel, list_length-1);
    Pixel_t *nextPixel_new = get_pixel_by_index(imageChunk,0);
    Pixel_t *replacedPixel = get_pixel_by_index(imageChunk, list_length-1);

    // Value is replaced
    assert_int_equal(replacedPixel->valor, newPixel.valor);
    // Index is replaced
    assert_int_equal(newPixel.index, list_length-1);
    // Next index left untouched
    assert_int_equal(nextPixel_old->index, nextPixel_new->index);
    assert_int_equal(nextPixel_old->valor, nextPixel_new->valor);
}

static void test_get_pixel_by_metadata_id(void **state){
    Pixel_t newPixel = {
        .valor = 13,
        .index = -1,
        .metadata_id = 20,
        .dirtyBit = false
    };
    
    replace_nth_pixel(imageChunk, &newPixel, 5);

    Pixel_t *foundPixel = get_pixel_by_metadata_id(imageChunk, 20);

    assert_true(foundPixel);
    assert_int_equal(foundPixel->valor, newPixel.valor);
    assert_int_equal(foundPixel->index, newPixel.index);

}

// int main(void) {
//     const struct CMUnitTest tests[] = {
//         cmocka_unit_test(create_list),
//         cmocka_unit_test_setup_teardown(test_add_first_item, setup_empty_list, teardown),
//         cmocka_unit_test_setup_teardown(test_add_item_at_end, setup_empty_list, teardown),
//         cmocka_unit_test_setup_teardown(test_add_several_items, setup_empty_list, teardown),
//         cmocka_unit_test(test_generate_fixed_length_chunk),
//         cmocka_unit_test_setup_teardown(test_replace_first_pixel, setup_populated_list, teardown),
//         cmocka_unit_test_setup_teardown(test_get_pixel_by_index, setup_populated_list, teardown),
//         cmocka_unit_test_setup_teardown(test_replace_nth_pixel, setup_populated_list, teardown),
//         // cmocka_unit_test_setup_teardown(test_replace_last_pixel, setup_populated_list, teardown),
//         cmocka_unit_test_setup_teardown(test_get_pixel_by_metadata_id, setup_populated_list, teardown),
//     };
//     return cmocka_run_group_tests(tests, NULL, NULL);
// }
