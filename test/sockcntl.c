#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>
#include <time.h>

struct timespec start_time, end_time;
int tmp, client_sock, client_sock2;

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

void SIGIO_handler(int sig_num){
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    printf("\033[1;32mSIGIO detected!\033[0m\n");
    printf("latency = %luns\n", diff(start_time, end_time));
    if(read(client_sock, &tmp, sizeof(tmp)) < 0){
        perror("read()");
        exit(1);
    }
    printf("return value is %d!\n", tmp);
    exit(0);
}

int main(int argc, char *argv[]){
    // Stack Variables
    int client_addr_len, client_addr_len2, init_fflags, ret_fflags;
    struct sockaddr_un client_addr, client_addr2;
    char *socket_file_path;
    int intr_data = 127;

    // Create UNIX Socket and Connect to Existing Socket File
    socket_file_path = argv[1];
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, socket_file_path);
    if(connect(client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0){
        perror("connect()");
        exit(1);
    }

    // Check O_ASYNC Support
    init_fflags = fcntl(client_sock, F_GETFL);
    printf("init_fflags: %#010x\n", init_fflags);
    printf("O_ASYNC: %#010x\n", O_ASYNC);
    printf("init_fflags & O_ASYNC: %#010x\n\n", init_fflags & O_ASYNC);
    fcntl(client_sock, F_SETFL, init_fflags | O_ASYNC); // O_ASYNC has to be set with fcntl()
    ret_fflags = fcntl(client_sock, F_GETFL);
    printf("ret_fflags: %#010x\n", ret_fflags);
    printf("O_ASYNC: %#010x\n", O_ASYNC);
    printf("ret_fflags & O_ASYNC: %#010x\n", ret_fflags & O_ASYNC);
    if (ret_fflags & O_ASYNC)
        printf("\033[1;32mThis file(socket) supports O_ASYNC flag!\033[0m\n\n");
    else
        printf("\033[1;31mThis file(socket) doesn\'t support O_ASYNC flag!\033[0m\n\n");

    // Register Signal Handler for SIGIO
    signal(SIGIO, SIGIO_handler);

    // Create and Connect Another Socket
    client_sock2 = socket(AF_UNIX, SOCK_STREAM, 0);
    bzero(&client_addr2, sizeof(client_addr2));
    client_addr2.sun_family = AF_UNIX;
    strcpy(client_addr2.sun_path, socket_file_path);
    if(connect(client_sock2, (struct sockaddr *)&client_addr2, sizeof(client_addr2)) < 0){ // it will trigger SIGIO if PID is already registered
        perror("connect()");
        exit(1);
    }

    // Set PID for SIGIO handling
    sleep(1); // delay for previous connect(client_sock2, ...) to finish
    fcntl(client_sock, F_SETOWN, getpid()); // registering PID of otherside process is mandatory for SIGIO to happen

    // Interrupt to Socket
    printf("sending value = %d\n\n", intr_data);
    write(client_sock2, &intr_data, sizeof(intr_data));
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Delay
    printf("Wait for 5 seconds maximum...\n");
    sleep(5);
    printf("\033[1;31mSIGIO not detected!\033[0m\n");
    if(read(client_sock, &tmp, sizeof(tmp)) < 0){
        perror("read()");
        exit(1);
    }
    printf("return value is %d!\n", tmp);

    // Terminate
    return 0;
}