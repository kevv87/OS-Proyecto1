#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <structs.h>
#include <constants.h>

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








    munmap(stat_ptr, sizeof(Stat_t));

    unlink(SM_STATS);

    close(fd);    

    return 0;

}

    







