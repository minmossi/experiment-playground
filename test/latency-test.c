#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define __USE_GNU
#include <sched.h>

struct timespec start_time, end_time;

unsigned long diff(struct timespec start, struct timespec end){
    struct timespec temp;
    if((end.tv_nsec - start.tv_nsec) < 0){
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = (long)1e9 + end.tv_nsec - start.tv_nsec;
    }
    else{
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp.tv_sec * (long)1e9 + temp.tv_nsec;
}

int main(){
    void *dest, *src;
    unsigned long mask = 1;
    if (sched_setaffinity(0, sizeof(mask), (cpu_set_t*)&mask) <0){
        perror("sched_setaffinity()");
        exit(1);
    }
    dest = malloc(4096*100);
    src = malloc(4096*100);

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    memcpy(dest, src, 4096*100);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    printf("latency = %luns\n", diff(start_time, end_time));

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    system("sudo cat /proc/wbinvd");
    memcpy(dest, src, 4096*100);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    printf("latency = %luns\n", diff(start_time, end_time));

    return 0;
}