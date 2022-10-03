#include "include/shared_memory.h"
#include "include/types.h"
#include "include/constants.h"
#include "include/imageChunk.h"
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int get_id(char * name, size_t size)
{
    key_t key = ftok(name, 0);
    int id = shmget(key, size, 0664 | IPC_CREAT | IPC_EXCL);
    if(id!=-1)
    {
        if(strcmp(name,CHUNK_LIST_PATH) == 0)
        {
            int pixel_quantity = (size-sizeof(ImageChunk_t))/sizeof(Node_t);

            void * shm_start;
            shm_start = obtain_shm_pointer(id, NULL);

            create_image_chunk(shm_start, pixel_quantity);

            ImageChunk_t *chunk_sh_ptr = (ImageChunk_t*) shm_start;

            ///Inicializa semaforo del chunk
            //todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            sem_init(&(chunk_sh_ptr->sem_encoders), 1, pixel_quantity);

            memcpy(shm_start, chunk_sh_ptr, sizeof(ImageChunk_t));

        }
        else if(strcmp(name, METADATA_PATH) == 0)
        {

            void * shm_start;
            shm_start = obtain_shm_pointer(id, NULL);

            Metadata_Table_t *metadata_sh_ptr = (Metadata_Table_t*) shm_start;

            ///Inicializa semaforo de metadata table
            sem_init(&(metadata_sh_ptr->sem_metadata_table), 1, 1);


        }
        else
        {
            int pixel_quantity = (size-sizeof(ImageChunk_t))/sizeof(Node_t);

            void * shm_start;
            shm_start = obtain_shm_pointer(id, NULL);

            Statistic_t *statistic_sh_ptr = (Statistic_t*) shm_start;

            ///Se guarda el valor del chunk y se inicializa los semaforos
            statistic_sh_ptr->chunk_size = pixel_quantity;
            sem_init(&(statistic_sh_ptr->semaphore_statistic), 1, 1);
            sem_init(&(statistic_sh_ptr->semaphore_visualizer), 1, 0);


        }
    }else{
        id = shmget(key, size, 0664 | IPC_CREAT);
    }

    return id;

}
void * write_shared_memory(void *dst_shm_ptr, void *src_ptr, size_t size){
    return memcpy(dst_shm_ptr, src_ptr, size);
}

int close_shm_ptr(int shmid, void * shm_ptr){
    // if(shmdt(shm_ptr) == -1){
    //     perror("shmdt failed");
    //     return -1;
    // }
    if(shmctl(shmid, IPC_RMID, NULL) == -1){
        perror("shmctl failed");
        return -1;
    }
    return 0;
}

void *obtain_shm_pointer(int shmid, void * map_ptr){
    void * shm_ptr = shmat(shmid, NULL, SHM_RND);
    if(shm_ptr==-1){
        perror("shmat failed");
        return NULL;
    }
    return shm_ptr;
}

void sem_down(Statistic_t *statistic_ptr, sem_t * semaphore) {

    clock_t sem_start;
    clock_t sem_end;

    sem_start = clock();

    sem_wait(semaphore);

    sem_end = clock();

    sem_wait(&(statistic_ptr->semaphore_statistic));

    statistic_ptr -> total_semaphore_waited_time += ((double)(sem_end - sem_start)) / CLOCKS_PER_SEC;

    sem_post(&(statistic_ptr->semaphore_statistic));

}

