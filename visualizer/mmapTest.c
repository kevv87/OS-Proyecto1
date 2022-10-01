#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define NAME "Shared_Memory"

int main() {

    int N = 5;

    int fd = shm_open(NAME, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if(fd == -1) {

        fd = shm_open(NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

        ftruncate(fd, N * sizeof(int));

    }

    int *ptr = mmap(NULL, N * sizeof(int),
    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    


    for(int i = 0; i < N; i++) {

        ptr[i] = i * 10;

    }

    for(int i = 0; i < N; i++) {

        printf("[%d]", ptr[i]);

    }

    /*
    int *ptr1 = mmap(0, N * sizeof(int),
    PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    
    if(ptr1 == MAP_FAILED) {

        printf("Mapping failed 0");

        return 1;

    }
    */

    /*
    for(int i = 0; i < N; i++) {

        printf("[%d]", ptr1[i]);

    }
    */
    
    
    printf("\n");
    /*
    for(int i = 0; i < N; i++) {

        printf("[%d]", ptr[i]);

    }
    */

    int num = 0;

    scanf("%d", &num);

    munmap(ptr, N * sizeof(int));

    printf("\n");

    unlink(NAME);

    close(fd);

    return 0;

}
