#include <cstring>
#include <chrono>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

using namespace std;

typedef struct connection{
    void *region;
    void *server;
    void *client;
    bool is_server;
    size_t req_size;
} Connection;

void *region_a;
void *region_a_start;
void *region_a_end;
void *region_a_bound;
void *region_b;
void *region_b_start;
void *region_b_end;
void *region_b_bound;

void *heap_a;
void *heap_b;
void *heap_c;
void *heap_d;

void *read_region(void *data) {
    void *region = ((Connection *)data)->region;
    void *heap_server = ((Connection *)data)->server;
    void *heap_client = ((Connection *)data)->client;
    size_t req_size = ((Connection *)data)->req_size;
    if (((Connection *)data)->is_server) {
        ;
    } else {
        ;
    }
}

void *write_region(void *data){
    void *region = ((Connection *)data)->region;
    void *heap_server = ((Connection *)data)->server;
    void *heap_client = ((Connection *)data)->client;
    size_t req_size = ((Connection *)data)->req_size;
    if (((Connection *)data)->is_server) {
        ;
    } else {
        ;
    }
}

// Variables
pthread_t thread_1;
pthread_t thread_2;
cpu_set_t single_cpu_1;
cpu_set_t single_cpu_2;
cpu_set_t full_cpu;
std::chrono::system_clock::time_point start;
std::chrono::duration<double> sec;

int main(int argc, char *argv[]){
    // Setup Variable
    size_t region_size = atoll(argv[1]);
    size_t heap_size = atoll(argv[2]);

    // Create Regions
    region_a = malloc(region_size);
    region_a_bound = region_a + region_size;
    region_a_start = region_a;
    region_a_end = region_a;
    region_b = malloc(region_size);
    region_b_bound = region_b + region_size;
    region_b_start = region_b;
    region_b_end = region_b;

    // Creater Writer/Reader Data
    heap_a = malloc(heap_size);
    heap_b = malloc(heap_size);
    heap_c = malloc(heap_size);
    heap_d = malloc(heap_size);

    // Open Random Device
    int rand_fd = open("/dev/random", O_RDONLY);
    
    // Make Full CPU Set
    CPU_ZERO(&single_cpu_1);
    CPU_SET(4, &single_cpu_1);
    CPU_SET(10, &single_cpu_1);
    CPU_ZERO(&single_cpu_2);
    CPU_SET(5, &single_cpu_2);
    CPU_SET(11, &single_cpu_2);
    
    // Make Full CPU Set
    CPU_ZERO(&full_cpu);
    CPU_SET(0, &full_cpu);
    CPU_SET(1, &full_cpu);
    CPU_SET(2, &full_cpu);
    CPU_SET(3, &full_cpu);
    CPU_SET(4, &full_cpu);
    CPU_SET(5, &full_cpu);
    CPU_SET(6, &full_cpu);
    CPU_SET(7, &full_cpu);
    CPU_SET(8, &full_cpu);
    CPU_SET(9, &full_cpu);
    CPU_SET(10, &full_cpu);
    CPU_SET(11, &full_cpu);

    // Set Full CPU Affinity for Main Process
    sched_setaffinity(0, sizeof(cpu_set_t), &full_cpu);



    // Test 1: Concurrent Communication with Pipelining and Affinity
    // A --Size of B--> B --Size of B--> C, in Different Affinity
    // Reset Regions and Generate Random Writer/Reader Data with Full Affinity
    memset(region_a, 0, region_size);
    memset(region_b, 0, region_size);
    read(rand_fd, heap_a, heap_size);
    read(rand_fd, heap_b, heap_size);
    read(rand_fd, heap_c, heap_size);
    read(rand_fd, heap_d, heap_size);

    // Set Timer
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    // Create Threads
    pthread_create();

    // Calculate Elapsed Time
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;



    // A --Size of B--> B --Size of B--> C, in Same Affinity
    // Reset Regions and Generate Random Writer/Reader Data with Full Affinity
    memset(region_a, 0, region_size);
    memset(region_b, 0, region_size);
    read(rand_fd, heap_a, heap_size);
    read(rand_fd, heap_b, heap_size);
    read(rand_fd, heap_c, heap_size);
    read(rand_fd, heap_d, heap_size);

    // Set Timer
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    // Create Threads
    pthread_create();

    // Calculate Elapsed Time
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;



    // A --Block Size --> B --Block Size--> C, in Different Affinity
    // Reset Regions and Generate Random Writer/Reader Data with Full Affinity
    memset(region_a, 0, region_size);
    memset(region_b, 0, region_size);
    read(rand_fd, heap_a, heap_size);
    read(rand_fd, heap_b, heap_size);
    read(rand_fd, heap_c, heap_size);
    read(rand_fd, heap_d, heap_size);

    // Evict Context
    sleep(1);

    // Set Timer
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    // Create Threads
    pthread_create();

    // Calculate Elapsed Time
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;



    // A --Block Size --> B --Block Size--> C, in Same Affinity
    // Reset Regions and Generate Random Writer/Reader Data with Full Affinity
    sched_setaffinity(0, sizeof(cpu_set_t), &full_cpu);
    sleep(1);
    memset(region_a, 0, region_size);
    memset(region_b, 0, region_size);
    read(rand_fd, heap_a, heap_size);
    read(rand_fd, heap_b, heap_size);
    read(rand_fd, heap_c, heap_size);
    read(rand_fd, heap_d, heap_size);

    // Set Affinity
    CPU_ZERO(&my_set);
    CPU_SET(5, &my_set);
    CPU_SET(11, &my_set);
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

    // Evict Context
    sleep(1);

    // Set Timer
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    // Create Threads
    pthread_create();

    // Calculate Elapsed Time
    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;



    // Test 2: Seperate TX/RX threads

    // Test 3: Flow Control

    return 0;
}