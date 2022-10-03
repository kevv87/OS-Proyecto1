#include "include/encoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include "common/include/imageChunk.h"
#include "common/include/constants.h"

#define SHM_CHUNK "/shm_chunk"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define KB 1024
#define INPUT_LIMIT 1*KB



//todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Borrar! (eventualmente)
//todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int id=1;
//todo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


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

Node_t * generate_descriptor(int encrypted_px, int px_position) {
    Node_t *node = malloc(sizeof(Node_t));
    node->value = encrypted_px;
    node->px_position = px_position;
    node->dirtyBit = 0;
    return node;
}

void insert_descriptor(Node_t *node, int pos) {
    node->index = pos;
    char * time_str = get_time();
    strncpy(node->insertion_time, time_str, 9);
    printf("Encrypted px: %x, Structure index: %d, Px position in file: %d, Insertion time: %s, Is read: %d\n",
           node->value, node->index, node->px_position, node->insertion_time, node->dirtyBit);
}


int main() {
    // Setting configuration
    ///Se extraen los valores
    char *image_path = malloc(sizeof(char)*INPUT_LIMIT);
    int *pixel_quantity = malloc(sizeof(int));
    int *execution_mode = malloc(sizeof(int));
    int *encryption_key = malloc(sizeof(int));
    get_encoder_params(image_path, pixel_quantity, execution_mode, encryption_key);

    ///Calcula el tammano del chunk
    int chunk_size = *pixel_quantity;
    size_t chunk_shm_size = sizeof(ImageChunk_t) + chunk_size * sizeof(Node_t);
    ///Crea las memorias compartidas
    int chunk_sh_id = get_id(CHUNK_LIST_PATH, chunk_shm_size);
    int metadata_sh_id = get_id(METADATA_PATH, METADATA_BLOCK_SIZE);
    int stat_sh_id = get_id(STATISTIC_PATH, STATISTIC_BLOCK_SIZE);

    ///Obtiene los punteros a memoria compartida
    ImageChunk_t* chunk_sh_ptr = malloc(sizeof(ImageChunk_t));
    obtain_shm_pointer(chunk_sh_id, chunk_sh_ptr);

    Metadata_Table_t* metadata_sh_ptr = malloc(sizeof(Metadata_Table_t));
    obtain_shm_pointer(metadata_sh_id, metadata_sh_ptr);

    Statistic_t* statistic_sh_ptr = malloc(sizeof(Statistic_t));
    obtain_shm_pointer(stat_sh_id, statistic_sh_ptr);

    ImgData_t *img_data = malloc(sizeof(ImgData_t));
    read_image(image_path, img_data);

    /// Incrementar numero de instancias
    sem_wait(&(statistic_sh_ptr->semaphore_statistic));

    statistic_sh_ptr -> total_instances += 1;

    sem_post(&(statistic_sh_ptr->semaphore_statistic));

    ///Hace down para reservar el uso de la tabla de metadata
    sem_down(statistic_sh_ptr, metadata_sh_ptr->sem_metadata_table);

    int metadata_id = -1;

    for(int i=0; i<MAX_METADATA_NODES; i++) {
        metadata_sh_ptr->metadata_array[i].has_decoder = 1;s_decoder = 0
            
            ///Se inicia el semaforo de lectura de imagen n
            sem_init(&(metadata_sh_ptr->metadata_array[i].image_semaphore), 1, 0);

            metadata_id = i;

            break;

    }    

    if(metadata_id == -1) {

        printf("Tabla de metadatos llena");

        return -1;

    }

    ///Hace up para liberar el uso de la tabla de metadata
    sem_post(&(metadata_sh_ptr->sem_metadata_table));

    if (img_data != NULL) {
        int px_position = 0;
        for(unsigned char *px_iter=img_data->img_ptr; px_iter!=(img_data->img_ptr + img_data->img_size); px_iter+=img_data->color_channels)
        {
            int hex_px = format_hex_px(*px_iter, *(px_iter+sizeof(unsigned char)), *(px_iter+2*sizeof(unsigned char)));
            Node_t *node = generate_descriptor(encrypt_pixel(hex_px, *encryption_key), 7);

            ///Encoders semaphore down
              Node_t *node = generate_descriptor(encrypt_pixel(hex_px, *encryption_key), 7);

            ///Encoders semaphore down
            sem_down(statistic_sh_ptr, chunk_sh_ptr->sem_encoders);

            ///Acces shared chunk
          sem_down(statistic_sh_ptr, chunk_sh_ptr->sem_encoders);

            ///Acces shared chunk
            Node_t * node_i = chunk_sh_ptr->head;
            while(node_i->dirtyBit){
                node_i = node_i->next;
            }

            insert_descriptor(node, node_i->index);

            /// Calcular tiempo de escritura
            clock_t sem_start;
            clock_t sem_end;

            sem_start = clock();

            /// Realizar escritura
            node_i->dirtyBit = true;
            node_i->metadata_id = metadata_id;
            node_i->value = node->value;
            node_i->px_position = px_position;
            char * time_str = get_time();
            strncpy(node_i->insertion_time , time_str, 9);

            sem_end = clock();

            sem_wait(&(statistic_sh_ptr->semaphore_statistic));

            /// Incrementar estadisticas
            statistic_sh_ptr -> total_kernel_time += ((double)(sem_end - sem_start)) / CLOCKS_PER_SEC;

            statistic_sh_ptr -> total_data_transfered += 1;

            if(node_i->value  >= 175) {

                statistic_sh_ptr ->total_pixels_175 += 1;

            }

            sem_post(&(statistic_sh_ptr->semaphore_statistic));

            /// Up al semaforo de lectura de la imagen N
            sem_post(&(metadata_sh_ptr->metadata_array[metadata_id].image_semaphore));

            px_position += 1;

            free(node);
        }//Aca termina la pasacion de pixeles

    }

    /// Decrementar numero de instancias
    sem_wait(&(statistic_sh_ptr->semaphore_statistic));
    statistic_sh_ptr -> total_instances -= 1;
    if(statistic_sh_ptr -> total_instances == 0) {
        sem_post(&(statistic_sh_ptr -> semaphore_visualizer));
    }
    sem_post(&(statistic_sh_ptr->semaphore_statistic));

    free(execution_mode);
    free(encryption_key);
    free(img_data);

    free(chunk_sh_ptr);
    free(metadata_sh_ptr);
    free(statistic_sh_ptr);
}


node_i->metadata_id == decod_cfg.metadata_id