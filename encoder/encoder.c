#include "include/encoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include "common/include/imageChunk.h"
#include "common/include/shared_memory.h"
#include "stb_image/stb_image.h"
#include "metadata_table.h"
#include "include/types.h"

#define SHM_CHUNK "/shm_chunk"
#define STB_IMAGE_IMPLEMENTATION
#define KB 1024
#define INPUT_LIMIT 1*KB


//todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Borrar! (eventualmente)
//todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int id=1;
//todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


void read_image(char *file_name, struct ImgData *img_data)
{
	printf("Opening image file: %s...\n", file_name);
    img_data->img_ptr = stbi_load(file_name, &img_data->width, &img_data->height, &img_data->color_channels, 0);
	if(img_data->img_ptr != NULL){
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

int format_hex_px(int red, int green, int blue)
{
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

struct Descriptor * generate_descriptor(uint16_t encrypted_px, int px_position) {
    struct Descriptor *desc = malloc(sizeof(struct Descriptor));
    desc->encrypted_px = encrypted_px;
    desc->px_position = px_position;
    desc->is_read = 0;
    return desc;
}

void insert_descriptor(struct Descriptor *desc, int pos)
{
    desc->struct_index = pos;
    char * time_str = get_time();
    strncpy(desc->insertion_time, time_str, 9);
    //(desc_array+(pos*sizeof(desc))) = desc;
    desc_array[pos] = *desc;
    printf("Encrypted px: %d, Structure index: %d, Px position in file: %d, Insertion time: %s, Is read: %d\n",
    desc->encrypted_px, desc->struct_index, desc->px_position, desc->insertion_time, desc->is_read);
}



int main()
{
    // Setting configuration
    char *image_path = malloc(sizeof(char)*INPUT_LIMIT);
    int *pixel_quantity = malloc(sizeof(int));
    int *execution_mode = malloc(sizeof(int));
    int *encryption_key = malloc(sizeof(int));
    get_encoder_params(image_path, pixel_quantity, execution_mode, encryption_key);


    //Creates ptr to shared memory
    //open shared memory
    int shm_size = sizeof(ImageChunk_t) + *pixel_quantity * sizeof(Node_t);
    int fd = obtain_shared_fd(SHM_CHUNK, true, shm_size);
    void * ptr_to_shm_start = obtain_shared_pointer(shm_size, fd);

    ImageChunk_t * chunk = (ImageChunk_t*) ptr_to_shm_start;
    if(chunk == NULL)
    {
        //Crea el chunk
        create_image_chunk(ptr_to_shm_start, *pixel_quantity);
        //sem_init(&(chunk->sem_encoders), 1, *pixel_quantity);
        sem_init(&(chunk->sem_encoders), 1, 10);

        sem_post(&(chunk->sem_encoders));
        sem_post(&(chunk->sem_encoders));
        sem_post(&(chunk->sem_encoders));
        sem_post(&(chunk->sem_encoders));
        sem_post(&(chunk->sem_encoders));


        //todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //Crea tabla de metadatos


        //todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //Crea estadisticas
    }


    struct ImgData *img_data = malloc(sizeof(struct ImgData));
    read_image(image_path, img_data);
    if (img_data != NULL)
    {

        shared_struct_metadata* shs_metadata = initialize_metadata_table();



        for(unsigned char *px_iter=img_data->img_ptr; px_iter!=(img_data->img_ptr + img_data->img_size); px_iter+=img_data->color_channels)
        {
            int hex_px = format_hex_px(*px_iter, *(px_iter+sizeof(unsigned char)), *(px_iter+2*sizeof(unsigned char)));
            struct Descriptor *desc = generate_descriptor(encrypt_pixel(hex_px, *encryption_key), 7);


            //int sem_value=100;
            //sem_getvalue(&(chunk->sem_encoders), &sem_value);
            //printf("Sem encoders value: %d\n", sem_value);


            //Encoders semaphore down
            //sem_wait(&(chunk->sem_encoders));
            //     /home/majinloop/Git/OS-Proyecto1/encoder/bw50.jpg










            //Acces shared chunk
            for(int i=0; i<chunk->size; i++)
            {
                Node_t * current_node = malloc(sizeof(Node_t));
                if(!get_pixel_by_index(chunk, i)->dirtyBit)
                {
                    insert_descriptor(desc, pos_counter);

                    //Valores del nodo
                    current_node->dirtyBit=true;

                    current_node->metadata_id=id;
                    current_node->value=desc->encrypted_px;
                    current_node->index=0;//Se le cae encima no se toca
                    current_node->next=NULL;


                    replace_nth_pixel(chunk, current_node, i);
                    //todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    //Up decoder-pair semaphore
                    //Recorrer metadata buscando el id
                    //sem_post(&(help));
                }
            }
        }//Aca termina la pasacion de pixeles



    }


    //Close shared memory
    close_shared_pointer(ptr_to_shm_start, shm_size);
    close_shared_memory(SHM_CHUNK);


    free(image_path);
    free(pixel_quantity);
    free(execution_mode);
    free(encryption_key);
    free(img_data);
}


