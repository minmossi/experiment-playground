#ifndef TEST_H
#define TEST_H

#include <cstdlib>
#include <mutex>

class PacketDescriptor{
public:
    void *packet_addr;
    size_t packet_size;
};

class Buffer{
private:
    // Packet Descriptors Method
    void push_packet_desc(PacketDescriptor *packet_desc);
    void pop_packet_desc(PacketDescriptor *packet_desc);
public:
    // Policy
    size_t max_packet_size;

    // Packet Descriptors
    std::mutex packet_descs_addr_mutex;
    size_t packet_descs_len;
    PacketDescriptor *packet_descs;
    PacketDescriptor *packet_descs_start_ptr;
    PacketDescriptor *packet_descs_end_ptr;

    // Buffer
    std::mutex bytes_addr_mutex;
    size_t bytes_size;
    void *bytes;
    void *bytes_start_ptr;
    void *bytes_end_ptr;

    // Constructor
    Buffer(size_t max_packet_size, size_t packet_descs_len, size_t bytes_size);

    // Deconstructor
    ~Buffer();

    // Policy Methods
    void change_max_packet_size(size_t max_packet_size);

    // Buffer Methods
    void push_data(void *src_addr, size_t size);
    void pop_data(void *dest_addr, size_t size);
};

#endif