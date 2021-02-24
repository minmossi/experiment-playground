#ifndef TEST_H
#define TEST_H

#include <cstdlib>
#include <mutex>

class PacketDescriptor{
public:
    int src_port;
    int dest_port;
    size_t packet_size;
    void *packet_addr;
};

class Buffer{
private:
    // Packet Descriptors Method
    void push_packet_desc(PacketDescriptor *packet_desc);
    void get_packet_desc(int dest_port, PacketDescriptor *packet_desc);
    void reduct_next_packet_size(int dest_port, size_t reduct_size);
    void pop_packet_desc(int dest_port);
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
    void push_data(int src_port, int dest_port, void *src_addr, size_t size);
    void pop_data(int src_port, int dest_port, void *dest_addr, size_t size);
    size_t get_next_packet_size(int src_port, int dest_port);
    void pop_packet(int src_port, int dest_port, void *dest_addr);
};

#endif