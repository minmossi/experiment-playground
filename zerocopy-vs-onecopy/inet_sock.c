#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
//#include <sched.h> // sched, affinity

int main(int argc, char *argv[])
{
    unsigned int ARRAY_SIZE = atoi(argv[1]);
    unsigned int WRITE_SIZE = atoi(argv[2]);
    unsigned int LOOP = atoi(argv[3]);
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

    int fd_urandom = open("/dev/urandom", O_RDONLY);
    if (fd_urandom < 0)
    {
        perror("open(/dev/urandom)");
        exit(1);
    }
    void *src, *dest;

    src = malloc(ARRAY_SIZE);
    dest = malloc(ARRAY_SIZE);

    // server socket init
    // server
    int server_socket;
    int client_socket;
    int client_addr_size;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("socket(server_socket)");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (-1 == bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        printf("bind() 실행 에러\n");
        exit(1);
    }

    if (-1 == listen(server_socket, 5))
    {
        printf("listen() 실행 실패\n");
        exit(1);
    }

    ssize_t bytes;
    for (int i = 0; i < LOOP; i++)
    {
        bytes = read(fd_urandom, src, WRITE_SIZE);
        if (bytes != WRITE_SIZE)
        {
            perror("read(/dev/urandom, src)");
            exit(1);
        }
        bytes = read(fd_urandom, dest, WRITE_SIZE);
        if (bytes != WRITE_SIZE)
        {
            perror("read(/dev/urandom, dest)");
            exit(1);
        }

        memcpy(dest, src, WRITE_SIZE);
        bytes = write(sock_client, src, WRITE_SIZE);
        if (bytes != WRITE_SIZE)
        {
            perror("write(sock_client)");
            exit(1);
        }
        bytes = read(sock_server_accept, dest, WRITE_SIZE);
        if (bytes != WRITE_SIZE)
        {
            perror("read(sock_server_accept)");
            exit(1);
        }
    }

    close(fd_urandom);
    free(src);
    free(dest);
    return 0;
}