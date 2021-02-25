#ifndef REGION_HH
#define REGION_HH

#include <cstdlib>

class Region{
    // Region
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
    void push_data(void *src_addr, size_t size);
    void pop_data(void *dest_addr, size_t size);
};

#endif