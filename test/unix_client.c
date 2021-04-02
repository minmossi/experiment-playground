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

unsigned long diff(struct timespec start, struct timespec end){
    struct timespec temp;
    if((end.tv_nsec - start.tv_nsec) < 0){
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = (long)1e9 + end.tv_nsec - start.tv_nsec;
    }
    else{
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp.tv_sec * (long)1e9 + temp.tv_nsec;
}


int main(int argc, char **argv)
{

    int client_len;
    int client_sockfd;

    FILE *fp_in;
    char buf_in[MAXLINE]; 
    char buf_get[MAXLINE]; 

    struct sockaddr_un clientaddr;

    if (argc != 2) {
        printf("Usage : %s [file_name]\n", argv[0]);
        exit(0);
    }

    client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sockfd == -1) {
        perror("error : ");
        exit(0);
    }

    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sun_family = AF_UNIX;
    strcpy(clientaddr.sun_path, argv[1]);
    client_len = sizeof(clientaddr);

    if (connect(client_sockfd, (struct sockaddr *)&clientaddr, client_len) < 0)
    {
        perror("Connect error: ");
        exit(0);
    }
    while(1)
    {
        memset(buf_in, 0x00, MAXLINE); 
        memset(buf_get, 0x00, MAXLINE); 
        printf("> "); 
        fgets(buf_in, MAXLINE, stdin);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        write(client_sockfd, &start_time, sizeof(start_time));
        read(client_sockfd, &start_time, sizeof(start_time));
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        printf("latency = %luns\n", diff(start_time, end_time));
    }

    close(client_sockfd);
    exit(0);
}