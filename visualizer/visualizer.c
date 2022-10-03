#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

#include "common/include/constants.h"
#include "common/include/shared_memory.h"

int main() {

    int stat_sh_id = get_id(STATISTIC_PATH, STATISTIC_BLOCK_SIZE);

    Statistic_t* statistic_sh_ptr = malloc(sizeof(Statistic_t));
    obtain_shm_pointer(stat_sh_id, statistic_sh_ptr);

    sem_wait(&(statistic_sh_ptr -> semaphore_visualizer));

    statistic_sh_ptr -> total_used_memory = 1000;
    statistic_sh_ptr -> total_semaphore_waited_time = 10.04;
    statistic_sh_ptr -> total_data_transfered = 5000;
    statistic_sh_ptr -> total_kernel_time = 25.07;
    statistic_sh_ptr -> total_pixels_175 = 2500;
    
    printf("-----------------------------------------------------------\n");
    printf("           Statistics              |       Values           \n");
    printf("-----------------------------------------------------------\n");
    printf("   Total used memory               |       %d\n", statistic_sh_ptr -> total_used_memory);
    printf("   Total semaphore waited time     |       %f\n", statistic_sh_ptr -> total_semaphore_waited_time);
    printf("   Total data transfered           |       %d\n", statistic_sh_ptr -> total_data_transfered);
    printf("   Total kernel time               |       %f\n", statistic_sh_ptr -> total_kernel_time);
    printf("   Total pixels greater than 175   |       %d\n", statistic_sh_ptr -> total_pixels_175);
    printf("------------------------------------------------------------\n");

    sem_post(&(statistic_sh_ptr -> semaphore_visualizer));

    close_shm_ptr(stat_sh_id, NULL);
    return 0;

}