#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
//#include <sched.h> // sched, affinity

int main(int argc, char *argv[]){
    unsigned int ARRAY_SIZE = atoi(argv[1]);
    unsigned int WRITE_SIZE = atoi(argv[2]);
    unsigned int LOOP = atoi(argv[3]);
    int ret;

    // sched, affinity
    // unsigned long mask = 1;
    // if (sched_setaffinity(0, sizeof(mask), (cpu_set_t*)&mask) <0){
    //     perror("sched_setaffinity()");
    //     exit(1);
    // }
    // struct sched_param sp;
    // memset(&sp, 0, sizeof(sp));
    // sp.sched_priority = 99;
    // if (sched_setscheduler(0, SCHED_FIFO, &sp)){
    //     perror("sched_setsched()");
    //     exit(1);
    // }

    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0){
        perror("open()");
        exit(1);
    }
    int fd_urandom = open("/dev/urandom", O_RDONLY);
    if (fd_urandom < 0){
        perror("open()");
        exit(1);
    }
    void *src, *buf, *dest;

    src = malloc(ARRAY_SIZE);
    buf = malloc(ARRAY_SIZE);
    dest = malloc(ARRAY_SIZE);

    for(int i = 0; i < LOOP; i++){  
        ret = read(fd_urandom, src, WRITE_SIZE);
        if(ret != WRITE_SIZE){
            perror("read()");
            exit(1);
        }
        ret = read(fd_urandom, dest, WRITE_SIZE);
        if(ret != WRITE_SIZE){
            perror("read()");
            exit(1);
        }

        memcpy(buf, src, WRITE_SIZE);
        memcpy(dest, buf, WRITE_SIZE);
    }

    free(src);
    free(buf);
    free(dest);
    return 0;
}