#ifndef RING_HH
#define RING_HH

#include <cstdlib>
#include <mutex>

// Packet Descriptor Struct
struct PacketDescriptor{
    int src_sock;
    int dst_sock;
    size_t packet_size;
    void *packet_addr;
};

// Ring Class
class Ring{
    // Policy
    size_t max_packet_size;

    // Packet Descriptors
    uint8_t packet_state;
    std::mutex packet_descs_ptr_mutex;
    size_t packet_descs_len;
    PacketDescriptor *packet_descs;
    PacketDescriptor *packet_descs_bound;
    PacketDescriptor *packet_descs_start_ptr;
    PacketDescriptor *packet_descs_end_ptr;

    // Ring
    uint8_t bytes_state;
    std::mutex bytes_ptr_mutex;
    size_t bytes_size;
    void *bytes;
    void *bytes_bound;
    void *bytes_start_ptr;
    void *bytes_end_ptr;

    // Packet Descriptors Method
    void push_packet_desc(PacketDescriptor *packet_desc);
    void get_packet_desc(int src_sock, int dst_sock, PacketDescriptor *packet_desc);
    void reduct_next_packet_size(int src_sock, int dst_sock, size_t reduct_size);
    void pop_packet_desc(int src_sock, int dst_sock);

public:
    // Constructor
    Ring(size_t max_packet_size, size_t packet_descs_len, size_t bytes_size);

    // Deconstructor
    ~Ring();

    // Policy Methods
    size_t get_ring_size() const;
    void reset_ring_size(size_t bytes_size); // Dangerous!
    void change_max_packet_size(size_t max_packet_size);

    // Ring Methods
    int push_data(int src_sock, int dst_sock, void *src_addr, size_t size);
    int pop_data(int src_sock, int dst_sock, void *dest_addr, size_t size);
    size_t get_next_packet_size(int src_sock, int dst_sock);
    int pop_packet(int src_sock, int dst_sock, void *dest_addr);
};

// Ring Group Class
class RingGroup{
public:
    // Ring Group
    Ring ring;
    Ring *next_ring;

    // Constructor
    RingGroup();

    // Deconstructor
    ~RingGroup();

    // Ring Group Methods


    // Ring Method
    int push_data(int src_sock, int dst_sock, void *src_addr, size_t size);
    int pop_data(int src_sock, int dst_sock, void *dest_addr, size_t size);
    size_t get_next_packet_size(int src_sock, int dst_sock);
    int pop_packet(int src_sock, int dst_sock, void *dest_addr);
};

#endif