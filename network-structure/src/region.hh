#ifndef REGION_HH
#define REGION_HH

#include <cstdlib>
#include <mutex>

class Region{
    // Region
    uint8_t region_state;
    std::mutex region_ptr_mutex;
    size_t bytes_size;
    void *bytes;
    void *bytes_bound;
    void *bytes_start_ptr;
    void *bytes_end_ptr;

public:
    // Constructor
    Region(size_t bytes_size);

    // Deconstructor
    ~Region();

    // Region Methods
    int push_data(void *src_addr, size_t size);
    int pop_data(void *dest_addr, size_t size);
};

class RegionGroup{
    // Region Group
    std::mutex region_group_mutex;
    Region region;
    Region *next_region;

public:
    // Constructor
    RegionGroup();

    // Deconstructor
    ~RegionGroup();

    // Region Group Methods
    int push_data(void *src_addr, size_t size);
    int pop_data(void *dest_addr, size_t size);
};

#endif