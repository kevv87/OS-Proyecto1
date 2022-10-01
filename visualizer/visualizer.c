#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "lib/structs.h"
#include "lib/constants.h"

int main() {

    int fd = shm_open(SM_STATS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if(fd == -1) {

        fd = shm_open(SM_STATS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

        ftruncate(fd, sizeof(Stat_t));

    }

    Stat_t *stat_ptr = mmap(NULL, sizeof(Stat_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(stat_ptr == MAP_FAILED) {

        munmap(stat_ptr, sizeof(Stat_t));

        unlink(SM_STATS);

        close(fd);   

        return 0;

    }

    sem_wait(&(stat_ptr -> semaphore_visualizer));

    /*
    stat_ptr -> total_used_memory = 1000;
    stat_ptr -> total_semaphore_waited_time = 10.04;
    stat_ptr -> total_data_transfered = 5000;
    stat_ptr -> total_kernel_time = 25.07;
    stat_ptr -> total_pixels_175 = 2500;
    */





    printf("-----------------------------------------------------------\n");
    printf("           Statistics              |       Values           \n");
    printf("-----------------------------------------------------------\n");
    printf("   Total used memory               |       %d\n", stat_ptr -> total_used_memory);
    printf("   Total semaphore waited time     |       %f\n", stat_ptr -> total_semaphore_waited_time);
    printf("   Total data transfered           |       %d\n", stat_ptr -> total_data_transfered);
    printf("   Total kernel time               |       %f\n", stat_ptr -> total_kernel_time);
    printf("   Total pixels greater than 175   |       %d\n", stat_ptr -> total_pixels_175);
    printf("------------------------------------------------------------\n");














    //sem_post(&(stat_ptr -> semaphore_visualizer));






    munmap(stat_ptr, sizeof(Stat_t));

    unlink(SM_STATS);

    close(fd);    

    return 0;

}

    







