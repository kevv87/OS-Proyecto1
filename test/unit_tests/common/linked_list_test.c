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

static void add_first_item(void **state){
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

static void add_item_at_end(void **state){
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
    assert_true(imageChunk->head->next->pixel = &newPixel);
    assert_true(imageChunk->tail->prev->pixel = &firstPixel);
        // Circularity validations
    assert_true(imageChunk->head->prev = imageChunk->tail);
    assert_true(imageChunk->tail->next = imageChunk->head);
}

static void generate_fixed_length_chunk(){
    ImageChunk_t imageChunk = create_image_chunk(10);

    assert_int_equal(imageChunk.size, 10);
}

// static void get_pixel_by_metadata_id(void **state){
//     ImageChunk_t imageChunk = create_image_chunk(imageChunk);
//     Pixel_t pixel;

//     for(int i = 0; i < 10; i++){
//         pixel = {
//             .valor = i,
//             .index = -1,
//             .metadata_id = i+3,
//             .dirtyBit = false
//         };
//         append_item(&imageChunk, pixel);
//     }



// }

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(create_list),
        cmocka_unit_test_setup_teardown(add_first_item, setup_empty_list, teardown),
        cmocka_unit_test_setup_teardown(add_item_at_end, setup_empty_list, teardown),
        cmocka_unit_test(generate_fixed_length_chunk)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
