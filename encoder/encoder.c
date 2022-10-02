#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define KB 1024
#define INPUT_LIMIT 1*KB

struct Descriptor {
    int encrypted_px;
    int struct_index;
    int px_position;
    int is_read;
    char insertion_time[9];
};
struct Descriptor desc_array[2500];

struct ImgData {
    int width, height, color_channels;
    unsigned char *img_ptr;
    size_t img_size;
};

void read_image(char *file_name, struct ImgData *img_data) {
	printf("Opening image file: %s...\n", file_name);
    img_data->img_ptr = stbi_load(file_name, &img_data->width, &img_data->height, &img_data->color_channels, 0);
	if(img_data->img_ptr != NULL){
        printf("%d, %d, %d\n\n\n", img_data->width, img_data->height, img_data->color_channels);
        img_data->img_size = img_data->width * img_data->height * img_data->color_channels;
		return;
	}
	printf("File couldn't be opened\n");
    return;
}

void get_encoder_params(char *image_path, int *px_quantity, int *mode, int *key) {
    printf("Specify image path: \n");
    fgets(image_path, INPUT_LIMIT, stdin);
    strtok(image_path, "\n");

    printf("Specify pixel quaintity for chunck: \n");
    scanf("%d", px_quantity);

    printf("Specify execution mode 0 for auto, 1 for manual: \n");
    scanf("%d", mode);

    printf("Specify encryption key: \n");
    scanf("%d", key);
}

int encrypt_pixel(int pixel, int key) {
    pixel = pixel ^ key;
    return pixel;
}

int format_hex_px(int red, int green, int blue) {
    printf("%d, %d, %d\n", red, green, blue);
    int hex_px = (red << 16) + (green << 8) + blue;
    return hex_px;
}

char * get_time() {
    time_t current_time;
    struct tm * time_info;
    char time_str[9];
    char * pointer = time_str;

    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", time_info);
    
    return pointer;
}

struct Descriptor * generate_descriptor(int encrypted_px, int px_position) {
    struct Descriptor *desc = malloc(sizeof(struct Descriptor));
    desc->encrypted_px = encrypted_px;
    desc->px_position = px_position;
    desc->is_read = 0;
    return desc;
}

void insert_descriptor(struct Descriptor *desc, int pos) {
    desc->struct_index = pos;
    char * time_str = get_time();
    strncpy(desc->insertion_time, time_str, 9);
    //(desc_array+(pos*sizeof(desc))) = desc;
    desc_array[pos] = *desc;
    printf("Encrypted px: %x, Structure index: %d, Px position in file: %d, Insertion time: %s, Is read: %d\n",
    desc->encrypted_px, desc->struct_index, desc->px_position, desc->insertion_time, desc->is_read);
}

int main() {
    // Setting configuration
    char *image_path = malloc(sizeof(char)*INPUT_LIMIT);
    int *pixel_quantity = malloc(sizeof(int));
    int *execution_mode = malloc(sizeof(int));
    int *encryption_key = malloc(sizeof(int));
    get_encoder_params(image_path, pixel_quantity, execution_mode, encryption_key);

    struct ImgData *img_data = malloc(sizeof(struct ImgData));
    read_image(image_path, img_data);
    if (img_data != NULL) {
        uint8_t is_run = 1;
        int pos_counter = 0;

        for (unsigned char *px_iter=img_data->img_ptr; px_iter!=(img_data->img_ptr + img_data->img_size); px_iter+=img_data->color_channels) {
            int hex_px = format_hex_px(*px_iter, *(px_iter+sizeof(unsigned char)), *(px_iter+2*sizeof(unsigned char)));
            struct Descriptor *desc = generate_descriptor(encrypt_pixel(hex_px, *encryption_key), pos_counter);
            if (is_run == 1) {
                insert_descriptor(desc, pos_counter);
            }
            else {
                sleep(0.7);
                px_iter -= img_data->color_channels;
                pos_counter -= 1;
            }
            pos_counter += 1;
            is_run *= -1;
        }
    }
    free(image_path);
    free(pixel_quantity);
    free(execution_mode);
    free(encryption_key);
    free(img_data);
}