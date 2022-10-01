#include <semaphore.h>
#include <time.h>

#include "lib/structs.h"
#include "lib/constants.h"

#include <stdio.h>

void sem_down(Stat_t *stat_ptr, sem_t semaphore) {

    clock_t sem_start;
    clock_t sem_end;

    sem_start = clock();

    sem_wait(&semaphore);

    sem_end = clock();

    stat_ptr -> total_semaphore_waited_time += ((double)(sem_end - sem_start)) / CLOCKS_PER_SEC;

}

void write(Stat_t *stat_ptr, void *shared_memory, void *value) {

    stat_ptr -> total_data_transfered ++;

    clock_t kernel_start;
    clock_t kernel_end;

    kernel_start = clock();

    // write logic

    kernel_end = clock();

    stat_ptr -> total_kernel_time += ((double)(kernel_end - kernel_start)) / CLOCKS_PER_SEC;

}

void write_pixel(Stat_t *stat_ptr, void *sm_linked_list, Pixel_t *pixel) {

    if(pixel -> value >= 175) {

        stat_ptr -> total_pixels_175 ++;

    }

    write(stat_ptr, sm_linked_list, pixel);

}

void read(Stat_t *stat_ptr, void *sm_linked_list) {

    clock_t kernel_start;
    clock_t kernel_end;

    kernel_start = clock();

    // read logic

    kernel_end = clock();

    stat_ptr -> total_kernel_time += ((double)(kernel_end - kernel_start)) / CLOCKS_PER_SEC;

}

int main() {

    printf("Hola");

    return 0;
}














