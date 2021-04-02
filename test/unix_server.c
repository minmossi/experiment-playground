#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAXLINE 1024

struct timespec start_time, end_time;

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

int main(int argc, char **argv)
{
    int server_sockfd, client_sockfd;
    int state, client_len;
    pid_t pid;

    FILE *fp;
    struct sockaddr_un clientaddr, serveraddr;

    char buf[MAXLINE];

    if (argc != 2)
    {
        printf("Usage : %s [socket file name]\n", argv[0]);
        exit(0);
    }

    if (access(argv[1], F_OK) == 0)
    {
        unlink(argv[1]);
    }

    client_len = sizeof(clientaddr);
    if ((server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error : ");
        exit(0);
    }
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, argv[1]);

    state = bind(server_sockfd, (struct sockaddr *)&serveraddr,
                 sizeof(serveraddr));
    if (state == -1)
    {
        perror("bind error : ");
        exit(0);
    }

    state = listen(server_sockfd, 5);
    if (state == -1)
    {
        perror("listen error : ");
        exit(0);
    }
    while (1)
    {
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr,
                               &client_len);
        pid = fork();
        if (pid == 0)
        {
            if (client_sockfd == -1)
            {
                perror("Accept error : ");
                exit(0);
            }

            while (1)
            {
                memset(buf, 0x00, MAXLINE);
                read(client_sockfd, &start_time, sizeof(start_time));
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                printf("latency = %luns\n", diff(start_time, end_time));
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                write(client_sockfd, &start_time, sizeof(start_time));
            }
        }
    }
    close(client_sockfd);
}