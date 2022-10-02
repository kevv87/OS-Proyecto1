#include <semaphore.h>
#include <time.h>

#include "lib/structs.h"
#include "lib/constants.h"

#include <stdio.h>

void sem_down(Statistic_t *statistic_ptr, sem_t semaphore) {

    clock_t sem_start;
    clock_t sem_end;

    sem_start = clock();

    sem_wait(&semaphore);

    sem_end = clock();

    statistic_ptr -> total_semaphore_waited_time += ((double)(sem_end - sem_start)) / CLOCKS_PER_SEC;

}

void write(Statistic_t *statistic_ptr, void *shared_memory, void *value) {

    statistic_ptr -> total_data_transfered ++;

    clock_t kernel_start;
    clock_t kernel_end;

    kernel_start = clock();

    // write logic

    kernel_end = clock();

    statistic_ptr -> total_kernel_time += ((double)(kernel_end - kernel_start)) / CLOCKS_PER_SEC;

}

void write_pixel(Statistic_t *statistic_ptr, void *sm_linked_list, Pixel_t *pixel) {

    if(pixel -> value >= 175) {

        statistic_ptr -> total_pixels_175 ++;

    }

    write(statistic_ptr, sm_linked_list, pixel);

}

void read(Statistic_t *statistic_ptr, void *sm_linked_list) {

    clock_t kernel_start;
    clock_t kernel_end;

    kernel_start = clock();

    // read logic

    kernel_end = clock();

    statistic_ptr -> total_kernel_time += ((double)(kernel_end - kernel_start)) / CLOCKS_PER_SEC;

}

void clean_memory() {

    // semaphores destory()


}



















int main() {

    printf("Main");

    return 0;

}