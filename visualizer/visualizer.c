#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "lib/structs.h"
#include "lib/constants.h"

#include "functions.c"

int main() {

    int fd = shm_open(SM_STATS, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if(fd == -1) {

        fd = shm_open(SM_STATS, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

        ftruncate(fd, sizeof(Stat_t));

    }

    Statistic_t *statistic_ptr = mmap(NULL, sizeof(SStatistic_ttat_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(statistic_ptr == MAP_FAILED) {

        munmap(statistic_ptr, sizeof(Statistic_t));

        unlink(SM_STATISTIC);

        close(fd);   

        return 0;

    }

    //sem_wait(&(statistic_ptr -> semaphore_visualizer));

    statistic_ptr -> total_used_memory = 1000;
    statistic_ptr -> total_semaphore_waited_time = 10.04;
    statistic_ptr -> total_data_transfered = 5000;
    statistic_ptr -> total_kernel_time = 25.07;
    statistic_ptr -> total_pixels_175 = 2500;
    
    printf("-----------------------------------------------------------\n");
    printf("           Statistics              |       Values           \n");
    printf("-----------------------------------------------------------\n");
    printf("   Total used memory               |       %d\n", statistic_ptr -> total_used_memory);
    printf("   Total semaphore waited time     |       %f\n", statistic_ptr -> total_semaphore_waited_time);
    printf("   Total data transfered           |       %d\n", statistic_ptr -> total_data_transfered);
    printf("   Total kernel time               |       %f\n", statistic_ptr -> total_kernel_time);
    printf("   Total pixels greater than 175   |       %d\n", statistic_ptr -> total_pixels_175);
    printf("------------------------------------------------------------\n");

    //sem_post(&(stat_ptr -> semaphore_visualizer));



    munmap(statistic_ptr, sizeof(Statistic_t));

    unlink(SM_STATS);

    close(fd);    

    return 0;

}

    







