#include <semaphore.h>
#include <time.h>

#include <structs.h>
#include <constants.h>

void sem_down(Stat_t *stat_ptr, sem_t *semaphore) {

    clock_t sem_start;
    clock_t sem_end;

    sem_start = clock();

    sem_wait(&semaphore);

    sem_end = clock();

    stat_ptr -> total_semaphore_wait += ((double)(sem_end - sem_start)) / CLOCKS_PER_SECOND;

}

void write(Stat_t *stat_ptr, void *sm_linked_list, void *value) {

    stat_ptr -> total_data_transfered ++;

    clock_t kernel_start;
    clock_t kernel_end;

    kernel_start = clock();

    // write logic

    kernel_end = clock();

    stat_ptr -> total_kernel_time += ((double)(kernel_end - kernel_start)) / CLOCKS_PER_SECOND;

}

void write_pixel(Stat_t *stat_ptr, void *sm_linked_list, Pixel_t *pixel) {

    if(pixel -> value >= 175) {

        stat_ptr -> total_pixels_175 ++;

    }

    write(stat_ptr, sm_linked_list, pixel) {

}

void read(Stat_t *stat_ptr, void *sm_linked_list) {

    clock_t kernel_start;
    clock_t kernel_end;

    kernel_start = clock();

    // read logic

    kernel_end = clock();

    stat_ptr -> total_kernel_time += ((double)(kernel_end - kernel_start)) / CLOCKS_PER_SECOND;

}
















// read or write here
    kernel_start = clock();

    if(read_flag == 1 && write_flag = 0) {

        


    } else {





    }