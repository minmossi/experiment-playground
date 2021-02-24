#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <chrono>

using namespace std;

void *region_1 = NULL;
void *region_2 = NULL;
void *region_3 = NULL;
void *heap_a = NULL;
void *heap_b = NULL;
void *heap_c = NULL;
void *heap_d = NULL;
void *heap_e = NULL;
void *heap_f = NULL;
void *heap_g = NULL;
void *heap_h = NULL;

int main(int argc, char *argv[]){
    size_t size_heap = atoll(argv[1]);
    size_t size_region = atoll(argv[2]);
    size_t loop_num = atoll(argv[3]);

    size_t malloc_bytes = size_heap * 8 + size_region * 3;
    printf("malloc() for %ld bytes... ", malloc_bytes);
    chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
    region_1 = malloc(size_region);
    region_2 = malloc(size_region);
    region_3 = malloc(size_region);

    heap_a = malloc(size_heap);
    heap_b = malloc(size_heap);
    heap_c = malloc(size_heap);
    heap_d = malloc(size_heap);
    heap_e = malloc(size_heap);
    heap_f = malloc(size_heap);
    heap_g = malloc(size_heap);
    heap_h = malloc(size_heap);
    chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    double malloc_byte_elapsed_ns = (double)chrono::duration_cast<std::chrono::nanoseconds>(end_time - begin_time).count() / malloc_bytes * 1024 * 1024;
    printf("Done - %lfns spent for 1MB of malloc()\n", malloc_byte_elapsed_ns);
    cout << "Total Time Elapsed for malloc(): " << chrono::duration_cast<std::chrono::nanoseconds>(end_time - begin_time).count() << "ns" << endl << endl;

    printf("Randomize... ");
    begin_time = std::chrono::steady_clock::now();
    for(size_t i = 0; i < size_heap; i++){
        *((uint8_t *)heap_a + i) = (uint8_t)rand();
        *((uint8_t *)heap_c + i) = (uint8_t)rand();
        *((uint8_t *)heap_e + i) = (uint8_t)rand();
        *((uint8_t *)heap_g + i) = (uint8_t)rand();
    }
    end_time = std::chrono::steady_clock::now();
    printf("Done\n");
    cout << "Total Time Elapsed for randomization: " << chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count() << "ms" << endl << endl;
    
    size_t remain, cnt;

    // Use Same Region
    double total_elapsed_time_sr = 0;
    for (int i = 0; i < loop_num; i++){
        remain = size_heap;
        cnt = 0;
        begin_time = std::chrono::steady_clock::now();
        while(remain){
            if(remain - size_region > 0){
                memcpy(region_1, heap_a + cnt, size_region);
                memcpy(heap_b + cnt, region_1, size_region);
                memcpy(region_1, heap_c + cnt, size_region);
                memcpy(heap_d + cnt, region_1, size_region);
                cnt += size_region;
                remain -= size_region;
            } else {
                memcpy(region_1, heap_a + cnt, remain);
                memcpy(heap_b + cnt, region_1, remain);
                memcpy(region_1, heap_c + cnt, remain);
                memcpy(heap_d + cnt, region_1, remain);
                cnt += remain;
                remain -= remain;
            }
        }
        end_time = std::chrono::steady_clock::now();
        total_elapsed_time_sr += chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
        cout << "[Same Region] Time Elapsed: " << chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count() << "ms" << endl;    
    }
    cout << endl << "Average Time Spent: " << total_elapsed_time_sr / loop_num << "ms" << endl << endl;

    // Use Different Region
    double total_elapsed_time_dr = 0;
    for (int i = 0; i < loop_num; i++){
        remain = size_heap;
        cnt = 0;
        begin_time = std::chrono::steady_clock::now();
        while(remain){
            if(remain - size_region > 0){
                memcpy(region_2, heap_e + cnt, size_region);
                memcpy(heap_f + cnt, region_2, size_region);
                memcpy(region_3, heap_g + cnt, size_region);
                memcpy(heap_h + cnt, region_3, size_region);
                cnt += size_region;
                remain -= size_region;
            } else {
                memcpy(region_2, heap_e + cnt, remain);
                memcpy(heap_f + cnt, region_2, remain);
                memcpy(region_3, heap_g + cnt, remain);
                memcpy(heap_h + cnt, region_3, remain);
                cnt += remain;
                remain -= remain;
            }
        }
        end_time = std::chrono::steady_clock::now();
        total_elapsed_time_dr += chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
        cout << "[Different Region] Time Elapsed: " << chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count() << "ms" << endl;
    }
    cout << endl << "Average Time Spent: " << total_elapsed_time_dr / loop_num << "ms" << endl << endl;

    cout << "Performance Difference(total_elapsed_time_dr / total_elapsed_time_sr): " << (total_elapsed_time_dr / total_elapsed_time_sr) * 100 -100 << "%" << endl;
    
    return 0;
}