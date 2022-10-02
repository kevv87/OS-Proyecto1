#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

struct Descriptor {
    int encrypted_px;
    int struct_index;
    int px_position;
    int is_read;
    char insertion_time[9];
};
struct Descriptor desc_array[2500];

struct Descriptor fetch_descriptor(int pos) {       // TODO: connect function to shared memory
    struct Descriptor desc = desc_array[pos];
    return desc;
}

void get_decoder_params(int *mode, int *key) {
    printf("Specify execution mode 0 for auto, 1 for manual: \n");
    scanf("%d", mode);

    printf("Specify encryption key: \n");
    scanf("%d", key);
}

int get_image_size() {      // TODO: fetch image size from meta data
    return 2500;
}

int decrypt_pixel(int pixel, int key) {
    pixel = pixel ^ key;
    return pixel;
}

int main() {
    // Setting configuration
    int IMAGE_SIZE = get_image_size();
    int *execution_mode = malloc(sizeof(int));
    int *decryption_key = malloc(sizeof(int));
    get_decoder_params(execution_mode, decryption_key);

    unsigned char *img_ptr = malloc(IMAGE_SIZE);
    unsigned char *px_iter;
    int px_counter = 0;
    int is_run = 1;
    while(px_counter < IMAGE_SIZE) {
        if (is_run == 1) {
            struct Descriptor desc = fetch_descriptor(px_counter);
            px_iter = img_ptr + desc.px_position;
            // Obtain RGB pixel values
            int R = desc.encrypted_px & 0xFF0000;
            int G = desc.encrypted_px & 0x00FF00;
            int B = desc.encrypted_px & 0x0000FF;
            //putpixel(px_counter%IMAGE_SIZE, px_counter/IMAGE_SIZE, desc.encrypted_px);        TODO: draw pixel
            // Insert px RGB in image file
            *px_iter = R;
            *(px_iter + 1) = G;
            *(px_iter + 2) = B;
            px_counter += 1;
        }
        else { sleep(0.7); }
        is_run *= -1;
    }
    sleep(3);

    // Decoding
    px_counter = 0;
    for (px_iter = img_ptr; px_iter!=(img_ptr+IMAGE_SIZE); px_iter+=3) {
        //int decrypted_px = decrypt_pixel(px_array[i], decryption_key);
        int R = decrypt_pixel(*px_iter, (*decryption_key & 0xFF0000));
        int G = decrypt_pixel(*(px_iter+1), (*decryption_key & 0x00FF00));
        int B = decrypt_pixel(*(px_iter+2), (*decryption_key & 0x0000FF));
        *px_iter = R;
        *(px_iter + 1) = G;
        *(px_iter + 2) = B;
        int decrypted_px = (R << 16) + (G << 8) + B;
        //putpixel(px_counter%IMAGE_SIZE, px_counter/IMAGE_SIZE, decrypted_px);     TODO: draw pixel
        px_counter += 1;
    }
    sleep(10);
}