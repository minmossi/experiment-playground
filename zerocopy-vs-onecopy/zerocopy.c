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
    const int PAGE_SIZE = getpagesize();
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
    void *src, *dest;

    src = malloc(ARRAY_SIZE);
    dest = malloc(ARRAY_SIZE);

#define PTE_BUF_LEN 512
    uint64_t *pte_buf = (uint64_t *)malloc(sizeof(uint64_t) * PTE_BUF_LEN);
    volatile uint64_t *pfn_src_lst = (uint64_t *)malloc(sizeof(uint64_t) * PTE_BUF_LEN);
    volatile uint64_t *pfn_dest_lst = (uint64_t *)malloc(sizeof(uint64_t) * PTE_BUF_LEN);
    uint64_t vfn_src, vfn_dest;
    ssize_t pte_src_readcnt, pte_dest_readcnt;

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

        vfn_src = ((uintptr_t)src / PAGE_SIZE);
        pte_src_readcnt = pread(fd, pte_buf, (WRITE_SIZE / PAGE_SIZE + 1) * sizeof(uint64_t), vfn_src * sizeof(uint64_t));
        if (pte_src_readcnt != (WRITE_SIZE / PAGE_SIZE + 1) * sizeof(uint64_t)){
            printf("pte_src_readcnt = %ld\n", pte_src_readcnt);
            perror("pread()");
            exit(1);
        }
        vfn_dest = ((uintptr_t)dest / PAGE_SIZE);
        pte_dest_readcnt = pread(fd, pte_buf, (WRITE_SIZE / PAGE_SIZE + 1) * sizeof(uint64_t), vfn_dest * sizeof(uint64_t));
        if (pte_dest_readcnt != (WRITE_SIZE / PAGE_SIZE + 1) * sizeof(uint64_t)){
            printf("pte_dest_readcnt = %ld\n", pte_dest_readcnt);
            perror("pread()");
            exit(1);
        }

        memcpy(dest, src, WRITE_SIZE);
    }

    free(src);
    free(dest);
    return 0;
}