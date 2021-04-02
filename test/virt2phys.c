#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
#include <fcntl.h> /* open */
#include <stdint.h> /* uint64_t  */
#include <stdlib.h> /* size_t */
#include <unistd.h> /* pread, sysconf */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sched.h>

struct timespec start_time, end_time, start_time_perf, end_time_perf;

unsigned long diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0)
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = (long)1e9 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp.tv_sec * (long)1e9 + temp.tv_nsec;
}

typedef struct {
    uint64_t pfn : 54;
    unsigned int soft_dirty : 1;
    unsigned int file_page : 1;
    unsigned int swapped : 1;
    unsigned int present : 1;
} PagemapEntry;

/* Parse the pagemap entry for the given virtual address.
 *
 * @param[out] entry      the parsed entry
 * @param[in]  pagemap_fd file descriptor to an open /proc/pid/pagemap file
 * @param[in]  vaddr      virtual address to get entry for
 * @return 0 for success, 1 for failure
 */

int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr)
{
    size_t nread;
    ssize_t ret;
    uint64_t data;

    nread = 0;
    while (nread < sizeof(data)) {
        ret = pread(pagemap_fd, ((uint8_t*)&data) + nread, sizeof(data) - nread,
                (vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
        nread += ret;
        if (ret <= 0) {
            return 1;
        }
    }
    entry->pfn = data & (((uint64_t)1 << 54) - 1);
    entry->soft_dirty = (data >> 54) & 1;
    entry->file_page = (data >> 61) & 1;
    entry->swapped = (data >> 62) & 1;
    entry->present = (data >> 63) & 1;
    return 0;
}


int main(int argc, char *argv[]){
    unsigned int ARRAY_SIZE = atoi(argv[1]);
    unsigned int WRITE_SIZE = atoi(argv[2]);
    unsigned int LOOP = atoi(argv[3]);
    int ret;
    printf("\n");

    // sched, affinity
    unsigned long mask = 1;
    if (sched_setaffinity(0, sizeof(mask), (cpu_set_t*)&mask) <0){
        perror("sched_setaffinity()");
        exit(1);
    }
    struct sched_param sp;
    memset(&sp, 0, sizeof(sp));
    sp.sched_priority = 99;
    if (sched_setscheduler(0, SCHED_FIFO, &sp)){
        perror("sched_setsched()");
        exit(1);
    }

    PagemapEntry *entry = malloc(sizeof(PagemapEntry));
    PagemapEntry *entry2 = malloc(sizeof(PagemapEntry));
    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0){
        perror("open()");
        exit(1);
    }

    int fd_urandom = open("/dev/urandom", O_RDONLY);
    void *src, *dest;

#ifdef USE_THP
    // madvise
    if (posix_memalign (&src, 4096, ARRAY_SIZE)){ // you need to align your memory first
        perror("posix_memalign()");
        exit(1);
    }
    if (posix_memalign (&dest, 4096, ARRAY_SIZE)){ // you need to align your memory first
        perror("posix_memalign()");
        exit(1);
    }
    if (madvise(src, ARRAY_SIZE, MADV_HUGEPAGE)){
        perror("madvise(src)");
        exit(1);
    }
    if (madvise(dest, ARRAY_SIZE, MADV_HUGEPAGE)){
        perror("madvise(dest)");
        exit(1);
    }
#else
    // mmap
    src = mmap(NULL, ARRAY_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (src == NULL-1){
        perror("mmap()");
        exit(1);
    }
    dest = mmap(NULL, ARRAY_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (dest == NULL-1){
        perror("mmap()");
        exit(1);
    }
#endif
    printf("initial src: %p\n", src);
    printf("initial dest: %p\n", dest);
    ret = pagemap_get_entry(entry2, fd, (uintptr_t)src);
    if (ret)
    {
        perror("initial pagemap_get_entry(src)");
        exit(1);
    }
    uint64_t tmp2 = entry2->pfn;
    printf("initial src pfn: %lu\n", tmp2);
    ret = pagemap_get_entry(entry, fd, (uintptr_t)src + 4096);
    if (ret)
    {
        perror("initial pagemap_get_entry(src+4096)");
        exit(1);
    }
    tmp2 = entry->pfn;
    printf("+4096 pfn: %lu\n", tmp2);
    printf("soft_dirty: %u\n", entry2->soft_dirty);
    printf("file_page: %u\n", entry2->file_page);
    printf("swapped: %u\n", entry2->swapped);
    printf("present: %u\n", entry2->present);
    ret = pagemap_get_entry(entry2, fd, (uintptr_t)dest);
    if (ret)
    {
        perror("initial pagemap_get_entry(dest)");
        exit(1);
    }
    tmp2 = entry2->pfn;
    printf("initial dest pfn: %lu\n", tmp2);
    ret = pagemap_get_entry(entry, fd, (uintptr_t)dest + 4096);
    if (ret)
    {
        perror("initial pagemap_get_entry(dest+4096)");
        exit(1);
    }
    tmp2 = entry->pfn;
    printf("+4096 pfn: %lu\n", tmp2);
    printf("soft_dirty: %u\n", entry2->soft_dirty);
    printf("file_page: %u\n", entry2->file_page);
    printf("swapped: %u\n", entry2->swapped);
    printf("present: %u\n", entry2->present);
    printf("\n");

    read(fd_urandom, src, ARRAY_SIZE);
    read(fd_urandom, dest, ARRAY_SIZE);
    printf("now src: %p\n", src);
    printf("now dest: %p\n", dest);
    ret = pagemap_get_entry(entry2, fd, (uintptr_t)src);
    if (ret)
    {
        perror("second pagemap_get_entry(src)");
        exit(1);
    }
    tmp2 = entry2->pfn;
    printf("now src pfn: %lu\n", tmp2);
    ret = pagemap_get_entry(entry, fd, (uintptr_t)src + 4096);
    if (ret)
    {
        perror("initial pagemap_get_entry(src+4096)");
        exit(1);
    }
    tmp2 = entry->pfn;
    printf("+4096 pfn: %lu\n", tmp2);
    printf("soft_dirty: %u\n", entry2->soft_dirty);
    printf("file_page: %u\n", entry2->file_page);
    printf("swapped: %u\n", entry2->swapped);
    printf("present: %u\n", entry2->present);
    ret = pagemap_get_entry(entry2, fd, (uintptr_t)dest);
    if (ret)
    {
        perror("second pagemap_get_entry(dest)");
        exit(1);
    }
    tmp2 = entry2->pfn;
    printf("now dest pfn: %lu\n", tmp2);
    ret = pagemap_get_entry(entry, fd, (uintptr_t)dest + 4096);
    if (ret)
    {
        perror("initial pagemap_get_entry(dest+4096)");
        exit(1);
    }
    tmp2 = entry->pfn;
    printf("+4096 pfn: %lu\n", tmp2);
    printf("soft_dirty: %u\n", entry2->soft_dirty);
    printf("file_page: %u\n", entry2->file_page);
    printf("swapped: %u\n", entry2->swapped);
    printf("present: %u\n", entry2->present);
    printf("\n");

    printf("---------------------------------------------------------------------\n\n");

    uint64_t tmp;
    void *src_prev = src, *dest_prev = dest;
    uint64_t src_pfn_prev, dest_pfn_prev;
    ret = pagemap_get_entry(entry, fd, (uintptr_t)src);
    if (ret)
    {
        perror("pagemap_get_entry(src)");
        exit(1);
    }
    src_pfn_prev = entry->pfn;
    ret = pagemap_get_entry(entry, fd, (uintptr_t)dest);
    if (ret)
    {
        perror("pagemap_get_entry(src)");
        exit(1);
    }
    dest_pfn_prev = entry->pfn;

    unsigned int score_2copy = 0;
    unsigned int score_1copy = 0;
    unsigned long long ttime_2copy = 0;
    unsigned long long ttime_1copy = 0;
    for(int i = 0; i < LOOP; i++){
        unsigned long tmp_ttime_2copy = 0;
        unsigned long tmp_ttime_1copy = 0;

        read(fd_urandom, src, WRITE_SIZE);
        read(fd_urandom, dest, WRITE_SIZE);

        clock_gettime(CLOCK_MONOTONIC, &start_time);
        ret = pagemap_get_entry(entry, fd, (uintptr_t)src);
        if (ret){
            perror("pagemap_get_entry(src)");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        printf("virt2phys(src) latency = %luns\n", diff(start_time, end_time));
        tmp_ttime_1copy += diff(start_time, end_time);
        printf("<MEMORY INFO>\n");
        tmp = entry->pfn;
        if (src_pfn_prev != tmp){
            printf("\033[1;31msrc pfn changed!\033[0m\n");
            //exit(2);
        }
        if (src_prev != src){
            printf("\033[1;31msrc virt addr changed!\033[0m\n");
            exit(3);
        }
        printf("pfn: %lu\n", tmp);
        ret = pagemap_get_entry(entry2, fd, (uintptr_t)src+4096);
        if (ret){
            perror("pagemap_get_entry(src+4096)");
            exit(1);
        }
        tmp = entry2->pfn;
        printf("+4096 pfn: %lu\n", tmp);
        printf("soft_dirty: %u\n", entry->soft_dirty);
        printf("file_page: %u\n", entry->file_page);
        printf("swapped: %u\n", entry->swapped);
        printf("present: %u\n", entry->present);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        ret = pagemap_get_entry(entry, fd, (uintptr_t)dest);
        if (ret){
            perror("pagemap_get_entry(dest)");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        printf("virt2phys(dest) latency = %luns\n", diff(start_time, end_time));
        tmp_ttime_1copy += diff(start_time, end_time);
        printf("<MEMORY INFO>\n");
        tmp = entry->pfn;
        if (dest_pfn_prev != tmp){
            printf("\033[1;31mdest pfn changed!\033[0m\n");
            //exit(2);
        }
        if (dest_prev != dest){
            printf("\033[1;31mdest virt addr changed!\033[0m\n");
            exit(3);
        }
        printf("pfn: %lu\n", tmp);
        ret = pagemap_get_entry(entry2, fd, (uintptr_t)dest+4096);
        if (ret){
            perror("pagemap_get_entry(dest+4096)");
            exit(1);
        }
        tmp = entry2->pfn;
        printf("+4096 pfn: %lu\n", tmp);
        printf("soft_dirty: %u\n", entry->soft_dirty);
        printf("file_page: %u\n", entry->file_page);
        printf("swapped: %u\n", entry->swapped);
        printf("present: %u\n\n", entry->present);

        clock_gettime(CLOCK_MONOTONIC, &start_time);
        memcpy(dest, src, WRITE_SIZE);
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        printf("memcpy(%u) latency = %luns\n\n", WRITE_SIZE, diff(start_time, end_time));
        tmp_ttime_2copy += 2 * diff(start_time, end_time);
        tmp_ttime_1copy += diff(start_time, end_time);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        ret = pagemap_get_entry(entry, fd, (uintptr_t)src);
        if (ret){
            perror("pagemap_get_entry(src)");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        printf("virt2phys(src) latency = %luns\n", diff(start_time, end_time));
        printf("<MEMORY INFO>\n");
        tmp = entry->pfn;
        if (src_pfn_prev != tmp){
            printf("\033[1;31msrc pfn changed!\033[0m\n");
            //exit(2);
        }
        if (src_prev != src){
            printf("\033[1;31msrc virt addr changed!\033[0m\n");
            exit(3);
        }
        printf("pfn: %lu\n", tmp);
        ret = pagemap_get_entry(entry2, fd, (uintptr_t)src+4096);
        if (ret){
            perror("pagemap_get_entry(src+4096)");
            exit(1);
        }
        tmp = entry2->pfn;
        printf("+4096 pfn: %lu\n", tmp);
        printf("soft_dirty: %u\n", entry->soft_dirty);
        printf("file_page: %u\n", entry->file_page);
        printf("swapped: %u\n", entry->swapped);
        printf("present: %u\n", entry->present);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        ret = pagemap_get_entry(entry, fd, (uintptr_t)dest);
        if (ret){
            perror("pagemap_get_entry(dest)");
            exit(1);
        }
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        printf("virt2phys(dest) latency = %luns\n", diff(start_time, end_time));
        printf("<MEMORY INFO>\n");
        tmp = entry->pfn;
        if (dest_pfn_prev != tmp){
            printf("\033[1;31mdest pfn changed!\033[0m\n");
            //exit(2);
        }
        if (dest_prev != dest){
            printf("\033[1;31mdest virt addr changed!\033[0m\n");
            exit(3);
        }
        printf("pfn: %lu\n", tmp);
        ret = pagemap_get_entry(entry2, fd, (uintptr_t)dest+4096);
        if (ret){
            perror("pagemap_get_entry(dest+4096)");
            exit(1);
        }
        tmp = entry2->pfn;
        printf("+4096 pfn: %lu\n", tmp);
        printf("soft_dirty: %u\n", entry->soft_dirty);
        printf("file_page: %u\n", entry->file_page);
        printf("swapped: %u\n", entry->swapped);
        printf("present: %u\n\n", entry->present);

        printf("tmp_ttime_2copy: %lu\n", tmp_ttime_2copy);
        printf("tmp_ttime_1copy: %lu\n\n", tmp_ttime_1copy);
        if (tmp_ttime_1copy <= tmp_ttime_2copy){
            score_1copy++;
            printf("1copy won!!!\n");
        }
        else{
            score_2copy++;
            printf("2copy won!\n");
        }
        ttime_2copy += tmp_ttime_2copy;
        ttime_1copy += tmp_ttime_1copy;

        printf("---------------------------------------------------------------------\n\n");
    }

    printf("ttime_2copy: %llu\n", ttime_2copy);
    printf("ttime_1copy: %llu\n", ttime_1copy);
    printf("ttime_2copy/ttime_1copy: %lf\n", (double)ttime_2copy/ttime_1copy);
    printf("score_2copy: %u\n", score_2copy);
    printf("score_1copy: %u\n", score_1copy);
    printf("\n");

#ifdef USE_THP
    free(src);
    free(dest);
    printf("USE_THP\n\n");
#else
    munmap(src, ARRAY_SIZE);
    munmap(dest, ARRAY_SIZE);
#endif
    return 0;
}